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

    uint8_t QcomGameConfiguration::RespId() const
    {
        return QCOM_EGMGCR_FC;
    }

    bool QcomGameConfiguration::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            uint32_t len = QCOM_GET_PACKET_LENGTH(sizeof(qc_egmgcrtype) - sizeof(p->Data.egmgcr.re));
            if (p->DLL.Length >= len &&
                p->DLL.Length >= (len + p->Data.egmgcr.NUM * p->Data.egmgcr.SIZ))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint8_t game_num = QCOM_MAX_GAME_NUM;
                uint8_t game = game_num;
                uint8_t flag = 0;
                uint8_t ivn = 0;
                uint8_t ivs[QCOM_REMAX_EGMGCR];

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    if (pd->data.config.games_num > 0)
                        game_num = pd->data.config.games_num;

                    for (game = 0; game < game_num; ++game)
                    {
                        if (pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_REQ)
                        {
                            if (!(pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN))
                            {
                                pd->data.games[game].gvn = p->Data.egmgcr.GVN;
                                pd->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_GVN;
                            }

                            if (pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET)
                            {
                                pd->data.games[game].lp_only = p->Data.egmgcr.FLG.bits.LPonly;
                                pd->data.games[game].var_hot_switching = p->Data.egmgcr.FLG.bits.hotswitching;
                                pd->data.games[game].plbm = p->Data.egmgcr.PLBM;
                                pd->data.games[game].variations.vnum = p->Data.egmgcr.NUM;

                                u32 variation = 0;
                                for (uint8_t v = 0; v < p->Data.egmgcr.NUM; ++v)
                                {
                                    qc_egmgcrretype *var = (qc_egmgcrretype*)((uint8_t*)(p->Data.egmgcr.re) +
                                        v * p->Data.egmgcr.SIZ);

                                    if (_QComGetBCD(&variation, &(var->VAR), sizeof(var->VAR)))
                                    {
                                        pd->data.games[game].variations.var[v] = static_cast<uint8_t>(variation);
                                        pd->data.games[game].variations.pret[v] = var->PRET;
                                    }
                                    else
                                    {
                                        ivs[ivn++] = v;
                                    }
                                }

                                pd->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_READY;
                            }

                            pd->data.control.game_config_state[game] &= ~QCOM_GAME_CONFIG_REQ;
                            flag = pd->data.control.game_config_state[game];
                            break;
                        }
                    }
                }

                if (game < game_num)
                {
                    if (ivn)
                    {
                        COMMS_START_LOG_BLOCK();

                        for (uint8_t i = 0; i < ivn; ++i)
                        {
                            qc_egmgcrretype *var = (qc_egmgcrretype*)((uint8_t*)(p->Data.egmgcr.re) + 
                                ivs[i] * p->Data.egmgcr.SIZ);

                            COMMS_LOG_BLOCK(
                                boost::format("EGM poll address %1% received variation value %2% which is not a BCD value") %
                                static_cast<uint32_t>(p->DLL.PollAddress) %
                                static_cast<uint32_t>(var->VAR), 
                                CLL_Error);
                        }

                        COMMS_END_LOG_BLOCK();

                        return false;
                    }

                    if (flag & QCOM_GAME_CONFIG_READY)
                    {
                        COMMS_LOG(boost::format("[%1%%%] game configuration ready\n") %
                            static_cast<uint32_t>((game + 1) * 100 / game_num), CLL_Info);
                    }
                    else
                    {
                        COMMS_LOG(boost::format("[%1%%%] game configuration GVN ready\n") %
                            static_cast<uint32_t>((game + 1) * 100 / game_num), CLL_Info);
                    }

                    return true;
                }
                else
                {
                    COMMS_LOG(boost::format("EGM poll address %1% received game configuration response but\
                        can't apply to any game of it.\n") % static_cast<uint32_t>(p->DLL.PollAddress) , CLL_Error);
                }
            }
        }

        return false;
    }

    bool QcomGameConfiguration::BuildGameConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameConfigData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            bool invalid_gvn = false;

            QcomDataPtr p = it->GetEgmData(poll_address);

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                uint8_t game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;
                uint8_t game = 0;
                if (!gvn)
                {
                    for (; game < game_num; ++game)
                    {
                        if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                            !(p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY))
                        {
                            gvn = p->data.games[game].gvn;
                            break;
                        }
                    }
                }
                else
                {
                    for (; game < game_num; ++game)
                    {
                        if (p->data.games[game].gvn == gvn)
                            break;
                    }
                }

                if (game != game_num)
                {
                    job->AddPoll(this->MakeGameConfigPoll(poll_address, p->data.control.last_control, gvn, data));

                    p->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_SET;
                    p->data.games[game].config = data;
                    p->data.games[game].gvn = gvn;

                    return true;
                }
                else
                {
                    invalid_gvn = true;
                }
            }

            if (invalid_gvn)
            {
                COMMS_LOG(boost::format("Can't set game configuration due to invalid GVN number : %1%\n") %
                    gvn, CLL_Error);
            }
        }

        return false;
    }




    QcomPollPtr QcomGameConfiguration::MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, 
        QcomGameConfigData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;

        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMGCP_FC;

        poll->poll.Data.egmgcp.GVN = gvn;
        _QComPutBCD(data.settings.var, &(poll->poll.Data.egmgcp.VAR), sizeof(poll->poll.Data.egmgcp.VAR));
        poll->poll.Data.egmgcp.GFLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmgcp.GFLG.bits.varlock = data.settings.var_lock;
        poll->poll.Data.egmgcp.GFLG.bits.GEF = data.settings.game_enable;
        poll->poll.Data.egmgcp.PGID = data.settings.pgid;
        poll->poll.Data.egmgcp.PNUM = data.progressive_config.pnum;
        poll->poll.Data.egmgcp.SIZ = sizeof(qc_egmcpretype);

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + 
            sizeof(poll->poll.Data.egmgcp) - sizeof(poll->poll.Data.egmgcp.re) + 
            poll->poll.Data.egmgcp.PNUM * poll->poll.Data.egmgcp.SIZ;

        for (uint8_t i = 0; i < data.progressive_config.pnum; ++i)
        {
            poll->poll.Data.egmgcp.re[i].PFLG.bits.res = 0;
            poll->poll.Data.egmgcp.re[i].PFLG.bits.LP = data.progressive_config.flag_p[i];
            poll->poll.Data.egmgcp.re[i].CAMT = data.progressive_config.camt[i];
        }

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
