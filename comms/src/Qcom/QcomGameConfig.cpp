#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomGameConfig.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{

    uint8_t QcomGameConfiguration::Id() const
    {
        return QCOM_EGMGCP_FC;
    }

    bool QcomGameConfiguration::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(buf);
        SG_UNREF_PARAM(length);

        return true;

    }

    void QcomGameConfiguration::BuildGameConfigPoll(std::vector<QcomGameConfigCustomData> const& data)
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

                    std::vector<uint8_t> lp;
                    std::vector<uint32_t> camt;

                    uint8_t pnum = d.data.pnum;

                    uint32_t loop;
                    for(loop = 0; loop < pnum; ++loop)
                    {
                        lp.push_back(d.data.lp[loop]);
                        camt.push_back(d.data.camt[loop]);
                    }

                    if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                        p->data.last_control ^= (QCOM_ACK_MASK);

                    job->AddPoll(this->MakeGameConfigPoll(d.egm, p->data.last_control,p->data.last_gvn, d.var,
                                                    d.var_lock, d.game_enable, pnum, lp, camt));

                    // store the data to game data
                    p->data.resp_funcode = QCOM_NO_RESPONSE;
                    p->data.progressive_config.pnum = pnum;
                    p->data.last_var = d.var;

                    for(loop = 0; loop < pnum; ++loop)
                    {
                        p->data.progressive_config.lp[loop] = d.data.lp[loop];
                        p->data.progressive_config.camt[loop] = d.data.camt[loop];
                    }

                }
            }

            it->AddJob(job);

        }
    }

    void QcomGameConfiguration::BuildGameConfigPoll(uint8_t poll_address, uint8_t var, uint8_t var_lock, uint8_t game_enable,
                                                    uint8_t pnum, const std::vector<uint8_t> &lp, const std::vector<uint32_t> &camt)
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

                job->AddPoll(this->MakeGameConfigPoll(poll_address, 
                    p->data.last_control, p->data.last_gvn, var, var_lock, game_enable, pnum,
                    lp, camt));

                p->data.resp_funcode = QCOM_NO_RESPONSE;
                p->data.progressive_config.pnum = pnum;
                p->data.last_var = var;

                uint32_t loop;
                for(loop = 0; loop < pnum; ++loop)
                {
                    p->data.progressive_config.lp[loop] = lp.at(loop);
                    p->data.progressive_config.camt[loop] = camt.at(loop);
                }

                it->AddJob(job);
            }
        }
    }

    QcomPollPtr QcomGameConfiguration::MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t last_gvn,
                                                          uint8_t var, uint8_t var_lock, uint8_t game_enable, uint8_t pnum,
                                                          std::vector<uint8_t> const&lp, std::vector<uint32_t> const&camt)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmgcptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMGCP_FC;

        poll->poll.Data.egmgcp.GVN = last_gvn;
        poll->poll.Data.egmgcp.VAR = var;
        poll->poll.Data.egmgcp.GFLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmgcp.GFLG.bits.varlock = var_lock;
        poll->poll.Data.egmgcp.GFLG.bits.GEF = game_enable;
        poll->poll.Data.egmgcp.PGID = last_gvn & var;
        poll->poll.Data.egmgcp.PNUM = pnum;
        poll->poll.Data.egmgcp.SIZ = sizeof(qc_egmcpretype);

        uint8_t loop;
        for(loop = 0; loop < lp.size() && loop < camt.size() && loop < pnum; ++loop)
        {
            poll->poll.Data.egmgcp.re[loop].PFLG.bits.res = 0;
            poll->poll.Data.egmgcp.re[loop].PFLG.bits.LP = lp.at(loop);
            poll->poll.Data.egmgcp.re[loop].CAMT = camt.at(loop);
        }

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
