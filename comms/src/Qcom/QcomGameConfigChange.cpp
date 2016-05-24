#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomGameConfigChange.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg {

    uint8_t QcomGameConfigurationChange::Id() const
    {
        return QCOM_EGMVCP_FC;
    }

    bool QcomGameConfigurationChange::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(buf);
        SG_UNREF_PARAM(length);

        return true;

    }

    void QcomGameConfigurationChange::BuildGameConfigChangePoll(std::vector<QcomGameConfigChangeCustomData> const& data)
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

                    job->AddPoll(this->MakeGameConfigChangePoll(d.egm, p->data.last_control,p->data.last_gvn, d.var,
                                                    d.game_enable));

                    // store the data to game data
                    p->data.resp_funcode = QCOM_NO_RESPONSE;
                    p->data.last_var = d.var;

                }
            }

            it->AddJob(job);

        }
    }

    void QcomGameConfigurationChange::BuildGameConfigChangePoll(uint8_t poll_address, uint8_t var, uint8_t game_enable)
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

                job->AddPoll(this->MakeGameConfigChangePoll(poll_address, p->data.last_control, p->data.last_gvn, var, game_enable));

                p->data.resp_funcode = QCOM_NO_RESPONSE;
                p->data.last_var = var;
                it->AddJob(job);
            }
        }
    }

    QcomPollPtr QcomGameConfigurationChange::MakeGameConfigChangePoll(uint8_t poll_address, uint8_t last_control, uint16_t last_gvn,
                                                          uint8_t var, uint8_t game_enable)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmvcptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMVCP_FC;

        poll->poll.Data.egmvcp.GVN = last_gvn;
        _QComPutBCD(var, &poll->poll.Data.egmvcp.VAR, sizeof(poll->poll.Data.egmvcp.VAR));
        poll->poll.Data.egmvcp.GFLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmvcp.GFLG.bits.GEF = game_enable;
        poll->poll.Data.egmvcp.PGID = poll->poll.Data.egmgcp.PGID;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
