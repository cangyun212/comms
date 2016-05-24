#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomPurgeEvents.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg {

    namespace {

        bool FindEGMDataBySER(uint32_t ser, QcomDataPtr p)
        {
			std::unique_lock<std::mutex> lock(p->locker);
            if (p->data.serialMidBCD == ser)
            {
                return true;
            }

            return false;
        }
    }

    uint8_t QcomPurgeEvents::Id() const
    {
        return QCOM_PEP_FC;
    }

    bool QcomPurgeEvents::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);
        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_peptype)))
            {
                QcomDataPtr pd = it->FindEgmData(std::bind(&FindEGMDataBySER, p->Data.egmcr2.SN.SER, std::placeholders::_1));

                if (pd)
                {
					std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.last_control ^= (QCOM_ACK_MASK);

                    pd->data.psn = p->Data.pepar.PPSN;

                    return true;
                }
            }
        }

        return false;

    }

    void QcomPurgeEvents::BuildPurgeEventsPoll(std::vector<QcomPurgeEventsCustomData> const& data)
    {
        if (auto it = m_qcom.lock())
        {
            bool pac_sent = false;

            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);
            for(auto const& d : data)
            {
                QcomDataPtr p = it->GetEgmData(d.egm);

                if (p)
                {
					std::unique_lock<std::mutex> lock(p->locker);

                    if (!pac_sent && p->data.poll_address == 0)
                    {
                        QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                        if (pb)
                        {
                            // TODO : it's better broadcast supply a function that
                            // it can just config specified egm poll address instead of all of them
                            pb->BuildPollAddressPoll();
                        }

                        pac_sent = true;
                    }

                    if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                        p->data.last_control ^= (QCOM_ACK_MASK);

                    job->AddPoll(this->MakePurgeEventsPoll(d.egm, p->data.last_control, d.psn,
                                                    d.evtno));

                    // store the data to game data
                    p->data.resp_funcode = QCOM_PEP_FC;

                }
            }

            it->AddJob(job);

        }
    }

    void QcomPurgeEvents::BuildPurgeEventsPoll(uint8_t poll_address, uint8_t psn, uint8_t evtno)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

				std::unique_lock<std::mutex> lock(p->locker);

                if (p->data.poll_address == 0)
                {
                    QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                    if (pb)
                    {
                        pb->BuildPollAddressPoll();
                    }
                }


                if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                    p->data.last_control ^= (QCOM_ACK_MASK);

                if (p->data.psn)
                    psn = p->data.psn;

                job->AddPoll(this->MakePurgeEventsPoll(poll_address, p->data.last_control, psn, evtno));

                p->data.resp_funcode = QCOM_NO_RESPONSE;
                it->AddJob(job);
            }
        }
    }

    QcomPollPtr QcomPurgeEvents::MakePurgeEventsPoll(uint8_t poll_address, uint8_t last_control, uint8_t psn,
                                                          uint8_t evtno)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_peptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_PEP_FC;

        poll->poll.Data.pep.PSN = psn;
        poll->poll.Data.pep.FLG = (uint8_t)0;
        poll->poll.Data.pep.EVTNO = evtno;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
