#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomProgramHashRequest.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomProgHashRequest::Id() const
    {
        return QCOM_PHRP_FC;
    }

    uint8_t QcomProgHashRequest::RespId() const
    {
        return QCOM_PHR_FC;
    }

    bool QcomProgHashRequest::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_phrtype2)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    std::memcpy(pd->data.control.hash, p->Data.psr2.HASH.hash, sizeof(pd->data.control.hash));

                    pd->data.control.egm_config_state |= QCOM_EGM_HASH_READY;

                    return true;
                }
            }
            else
            {
                COMMS_LOG(boost::format("Program Hash Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }

    bool QcomProgHashRequest::BuildProgHashRequstPoll(QcomJobDataPtr & job, uint8_t poll_address, QcomProgHashRequestData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeProgHashRequestPoll(poll_address, p->data.control.last_control, data));

                p->data.control.machine_eable = data.mef;

                if (data.new_seed)
                {
                    std::memcpy(p->data.control.seed, data.seed, sizeof(p->data.control.seed));
                }

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomProgHashRequest::MakeProgHashRequestPoll(uint8_t poll_address, uint8_t last_control, QcomProgHashRequestData const & data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_PHRP_FC;

        poll->poll.Data.phrp2.RES.bits.MEF = data.mef;
        poll->poll.Data.phrp2.RES.bits.seed = data.new_seed;

        if (data.new_seed)
        {
            std::memcpy(poll->poll.Data.phrp2.SEED.seed, data.seed, sizeof(poll->poll.Data.phrp2.SEED.seed));
            poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_phrptype2);
        }
        else
        {
            poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_phrptype2) - sizeof(qc_phrp2seedtype);
        }

        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}



