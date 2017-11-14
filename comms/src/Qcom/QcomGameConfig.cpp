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
                uint8_t gvn_status_num = 0;
                uint8_t gvn_status[QCOM_MAX_GAME_NUM];
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
                        if (pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET)
                        {
                            if (pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN)
                            {
                                if (pd->data.games[game].gvn == p->Data.egmgcr.GVN)
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
                                    flag = pd->data.control.game_config_state[game];
                                    break;
                                }
                            }
                            else
                            {
                                gvn_status[gvn_status_num++] = game;
                            }
                        }
                        else if (pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_REQ)
                        {
                            if (!(pd->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN))
                            {
                                pd->data.games[game].gvn = p->Data.egmgcr.GVN;
                                pd->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_GVN;
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

                    if (gvn_status_num)
                    {
                        COMMS_START_LOG_BLOCK();

                        for (uint8_t i = 0; i < gvn_status_num; ++i)
                        {
                            COMMS_LOG_BLOCK(boost::format("Game %1% is set but no available GVN received\n") %
                                            static_cast<uint32_t>(gvn_status[i]), CLL_Error);
                        }

                        COMMS_END_LOG_BLOCK();
                    }

                    if (flag & QCOM_GAME_CONFIG_READY)
                    {
                        COMMS_LOG(boost::format("Game configuration of Game %1% (Game Number: %2%) is ready\n") %
                            static_cast<uint32_t>(game) % static_cast<uint32_t>(game_num), CLL_Info);
                    }
                    else
                    {
                        COMMS_LOG(boost::format("Game configuration GVN of Game %1% (Game Number: %2%) is ready\n") %
                            static_cast<uint32_t>(game) % static_cast<uint32_t>(game_num), CLL_Info);
                    }

                    return true;
                }
                else
                {
                    COMMS_LOG(boost::format("EGM poll address %1% received game configuration response but\
                        can't apply to any game of it.\n") % static_cast<uint32_t>(p->DLL.PollAddress) , CLL_Error);
                }
            }
            else
            {
                COMMS_LOG(boost::format("Game Configuration Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }

    bool QcomGameConfiguration::BuildGameConfigPollForGame(QcomJobDataPtr &job, QcomDataPtr &p, uint8_t poll_address, uint8_t game, uint8_t pnum, const QcomGameConfigData &data)
    {
        job->AddPoll(this->MakeGameConfigPoll(poll_address, p->data.control.last_control, p->data.games[game].gvn, pnum, data));

        p->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_SET;
        //p->data.games[game].config = data;
        p->data.games[game].config.settings = data.settings;
        for (uint8_t i = 0; i < pnum; ++i)
        {
            p->data.games[game].config.progressive.sup[i] = data.progressive.sup[i];
            p->data.games[game].config.progressive.flag_p[i] = data.progressive.flag_p[i];
        }

        p->data.games[game].prog.pnum = pnum;

        return true;
    }

    bool QcomGameConfiguration::BuildGameConfigJobs(std::vector<QcomJobDataPtr> &jobs, uint8_t poll_address, uint16_t gvn, uint8_t pnum, const QcomGameConfigData &data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                uint8_t game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;
                uint8_t game = 0;

                jobs.clear();
                if (!gvn)
                {

                    for (; game < game_num; ++game)
                    {
                        if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                            !(p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY))
                        {
                            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);
                            this->BuildGameConfigPollForGame(job, p, poll_address, game, pnum, data);
                            jobs.push_back(job);
                        }
                    }
                }
                else
                {
                    for (; game < game_num; ++game)
                    {
                        if (p->data.games[game].gvn == gvn)
                        {
                            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);
                            this->BuildGameConfigPollForGame(job, p, poll_address, game, pnum, data);
                            jobs.push_back(job);
                            break;
                        }
                    }
                }
            }

            if (!jobs.empty())
            {
                return true;
            }
            else
            {
                if (!gvn)
                {
                    COMMS_LOG("Can't set game configuration, please configure EGM first.", CLL_Error);
                }
                else
                {
                    COMMS_LOG(boost::format("Can't set game configuration due to invalid GVN number : 0x%|04X|.") %
                              gvn, CLL_Error);
                }
            }
        }

        return false;
    }

    //bool QcomGameConfiguration::BuildGameConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameConfigData const & data)
    //{
    //    if (auto it = m_qcom.lock())
    //    {
    //        QcomDataPtr p = it->GetEgmData(poll_address);

    //        if (p)
    //        {
    //            std::unique_lock<std::mutex> lock(p->locker);

    //            uint8_t game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;
    //            uint8_t game = 0;
    //            if (!gvn)
    //            {
    //                for (; game < game_num; ++game)
    //                {
    //                    if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
    //                        !(p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY))
    //                    {
    //                        break;
    //                    }
    //                }
    //            }
    //            else
    //            {
    //                for (; game < game_num; ++game)
    //                {
    //                    if (p->data.games[game].gvn == gvn)
    //                    {
    //                        break;
    //                    }
    //                }
    //            }

    //            if (game < game_num)
    //            {
    //                job->AddPoll(this->MakeGameConfigPoll(poll_address, p->data.control.last_control, p->data.games[game].gvn, data));

    //                p->data.control.game_config_state[game] |= QCOM_GAME_CONFIG_SET;
    //                p->data.games[game].config = data;

    //                return true;
    //            }
    //        }

    //        if (!gvn)
    //        {
    //            COMMS_LOG("Can't set game configuration, please configure EGM first.", CLL_Error);
    //        }
    //        else
    //        {
    //            COMMS_LOG(boost::format("Can't set game configuration due to invalid GVN number : %1%.") %
    //                      gvn, CLL_Error);
    //        }
    //    }

    //    return false;
    //}

    QcomPollPtr QcomGameConfiguration::MakeGameConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, uint8_t pnum, 
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
        poll->poll.Data.egmgcp.PNUM = pnum; //data.progressive_config.pnum;
        poll->poll.Data.egmgcp.SIZ = sizeof(qc_egmcpretype);

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + 
            sizeof(poll->poll.Data.egmgcp) - sizeof(poll->poll.Data.egmgcp.re) + 
            poll->poll.Data.egmgcp.PNUM * poll->poll.Data.egmgcp.SIZ;

        for (uint8_t i = 0; i < pnum; ++i)
        {
            poll->poll.Data.egmgcp.re[i].PFLG.bits.res = 0;
            poll->poll.Data.egmgcp.re[i].PFLG.bits.LP = data.progressive.flag_p[i];
            poll->poll.Data.egmgcp.re[i].CAMT = data.progressive.sup[i];
        }

        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
