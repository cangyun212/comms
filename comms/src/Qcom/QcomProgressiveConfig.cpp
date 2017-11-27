#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomProgressiveConfig.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomProgressiveConfig::Id() const
    {
        return QCOM_PCP_FC;
    }

    uint8_t QcomProgressiveConfig::RespId() const
    {
        return QCOM_PCR_FC;
    }

    bool QcomProgressiveConfig::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(QCOM_PCR_LENGTH(p)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);
                uint8_t game_num = QCOM_MAX_GAME_NUM;
                uint8_t game = 0;

                uint8_t ivarbcd = 0;
                uint8_t ivar = 0;
                uint32_t nomvar = 0;
                uint8_t ipnum = 0;
                uint8_t inum = 0;
                uint8_t ipnums[QCOM_REMAX_PCP];
                uint8_t icustomSAP[QCOM_REMAX_PCP];
                //uint32_t isups[QCOM_REMAX_PCP];
                //uint8_t saplp[QCOM_REMAX_PCP];
                QcomProgressiveConfigData iprog;

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    if (pd->data.config.games_num > 0)
                        game_num = pd->data.config.games_num;

                    for (; game < game_num; ++game)
                    {
                        if (pd->data.games[game].gvn == p->Data.pcr.GVN)
                            break;
                    }

                    if (game < game_num)
                    {
                        QcomGameData &gc = pd->data.games[game];

                        u32 variation = 0;
                        if (_QComGetBCD(&variation, &(p->Data.pcr.VAR), sizeof(p->Data.pcr.VAR)))
                        {
                            if (gc.config.settings.var == static_cast<uint8_t>(variation))
                            {
                                gc.customSAP = p->Data.pcr.NUM.bits.customSAP;

                                if (gc.prog.pnum != p->Data.pcr.NUM.bits.num)
                                {
                                    ipnum = gc.prog.pnum;
                                    gc.prog.pnum = p->Data.pcr.NUM.bits.num;
                                }

                                for (uint8_t i = 0; i < gc.prog.pnum; ++i)
                                {
                                    qc_pcrretype *pcrre = (qc_pcrretype*)(((uint8_t*)(&p->Data.pcr)) + sizeof(qc_pcrtype) +
                                                                          i * p->Data.pcr.SIZ);

                                    //if ((pd->data.control.game_config_state[game] & QCOM_GAME_PC_CHANGE))
                                    //{
                                    if (pcrre->SUP != gc.config.progressive.sup[i] ||
                                        pcrre->PLF.bits.saplp != gc.config.progressive.flag_p[i] ||
                                        pcrre->PINC != gc.config.progressive.pinc[i] ||
                                        pcrre->CEIL != gc.config.progressive.ceil[i] ||
                                        pcrre->AUXRTP != gc.config.progressive.auxrtp[i])
                                    {
                                        ipnums[inum] = i;

                                        if ((pcrre->PLF.bits.saplp == gc.config.progressive.flag_p[i]) &&
                                            (!pcrre->PLF.bits.saplp) &&
                                            (!gc.customSAP))
                                            icustomSAP[inum] = 1;
                                        else
                                            icustomSAP[inum] = 0;

                                        iprog.sup[inum] = gc.config.progressive.sup[i];
                                        iprog.flag_p[inum] = gc.config.progressive.flag_p[i];
                                        iprog.pinc[inum] = gc.config.progressive.pinc[i];
                                        iprog.ceil[inum] = gc.config.progressive.ceil[i];
                                        iprog.auxrtp[inum] = gc.config.progressive.auxrtp[i];

                                        ++inum;
                                    }
                                    //}

                                    gc.config.progressive.sup[i] = pcrre->SUP;
                                    gc.config.progressive.flag_p[i] = pcrre->PLF.bits.saplp;
                                    gc.config.progressive.pinc[i] = pcrre->PINC;
                                    gc.config.progressive.ceil[i] = pcrre->CEIL;
                                    gc.config.progressive.auxrtp[i] = pcrre->AUXRTP;
                                }

                                if (pd->data.control.game_config_state[game] & QCOM_GAME_PC_CHANGE)
                                    pd->data.control.game_config_state[game] &= ~QCOM_GAME_PC_CHANGE;
                            }
                            else
                            {
                                nomvar = variation;
                                ivar = gc.config.settings.var;
                                //gc.customSAP = p->Data.pcr.NUM.bits.customSAP;
                            }
                        }
                        else
                        {
                            ivarbcd = p->Data.pcr.VAR;
                        }

                        if (!ipnum && !inum && !ivar && !ivarbcd)
                        {
                            COMMS_LOG("Progressive configuration Response received.\n", CLL_Info);
                            return true;
                        }
                    }
                }

                if (game >= game_num)
                {
                    COMMS_LOG(boost::format("Can't find Game(GVN: 0x%|04X|) for the progressive config change\n") % 
                        p->Data.pcr.GVN, CLL_Error);
                }
                else if (ivarbcd)
                {
                    COMMS_LOG(boost::format("Invalid VAR BCD %|| of Game(GVN: 0x%|04X|) received for progressive config change\n")%
                        static_cast<uint32_t>(ivarbcd) % p->Data.pcr.GVN, CLL_Error);
                }
                else if (ivar)
                {
                    COMMS_LOG(boost::format("VAR %|02d| is not current VAR %|02d| of Game(GVN: 0x%|04X|) received for progressive config change\n") %
                        nomvar % static_cast<uint32_t>(ivar) % p->Data.pcr.GVN, CLL_Error);
                }
                else if (ipnum)
                {
                    COMMS_LOG(boost::format("EGM report Game(GVN: 0x%|04X|) has %|| progressive levels instead of\
                        %|| levels which reported by local data\n") % 
                        p->Data.pcr.GVN % (unsigned int)p->Data.pcr.NUM.bits.num % (unsigned int)ipnum
                        , CLL_Error);
                }
                else if (inum)
                {
                    COMMS_START_LOG_BLOCK();

                    for (uint8_t i = 0; i < inum; ++i)
                    {
                        uint8_t ip = ipnums[i];
                        qc_pcrretype *pcrre = (qc_pcrretype*)(((uint8_t*)(&p->Data.pcr)) + sizeof(qc_pcrtype) +
                            ip * p->Data.pcr.SIZ);

                        if (!icustomSAP[i])
                        {
                            COMMS_LOG_BLOCK(boost::format("Progressive configuration for Game(GVN 0x%|04X|) Level %|| mismatch.\n") %
                                p->Data.pcr.GVN % static_cast<uint32_t>(i), CLL_Error);
                            COMMS_LOG_BLOCK(boost::format("EGM:\tSUP %||,\tTYPE %||,\tPINC %||,\tCEIL %||,\tAUXRTP %||.\n") %
                                pcrre->SUP % (pcrre->PLF.bits.saplp ? "LP" : "SAP") % pcrre->PINC % pcrre->CEIL % pcrre->AUXRTP, CLL_Error);
                            COMMS_LOG_BLOCK(boost::format("LOCAL:\tSUP %||,\tTYPE %||,\tPINC %||,\tCEIL %||.\tAUXRTP %||.\n") %
                                iprog.sup[i] % (iprog.flag_p[i] ? "LP" : "SAP") % iprog.pinc[i] % iprog.ceil[i] % iprog.auxrtp[i], CLL_Error);
                            COMMS_LOG_BLOCK("\n", CLL_Error);
                        }
                        else
                        {
                            COMMS_LOG_BLOCK(boost::format("Custom SAP is not suppport for Game(GVN 0x%|04X|) Level %||\n") %
                                p->Data.pcr.GVN % static_cast<uint32_t>(i), CLL_Error);
                        }
                    }

                    COMMS_END_LOG_BLOCK();
                }
            }
            else
            {
                COMMS_LOG(boost::format("Progressive configuration Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }

    bool QcomProgressiveConfig::BuildProgConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const & data, uint16_t &pgid)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            uint8_t game_num = 0;
            uint8_t game = 0;
            uint8_t ipnum = 0;
            bool istate = false;
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;

                for (; game < game_num; ++game)
                {
                    if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                        p->data.games[game].gvn == gvn)
                    {
                        if (p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY)
                        {
                            if (p->data.games[game].prog.pnum == pnum) // Is level number able to be changed? 
                            {
                                job->AddPoll(this->MakeProgConfigPoll(poll_address, p->data.control.last_control, gvn, pnum, data));

                                p->data.control.game_config_state[game] |= QCOM_GAME_PC_CHANGE;

                                if (!p->data.config.shared_progressive)
                                {
                                    QcomGameData &gc = p->data.games[game];
                                    for (uint8_t i = 0; i < pnum; ++i)
                                    {
                                        gc.config.progressive.sup[i] = data.sup[i];
                                        gc.config.progressive.pinc[i] = data.pinc[i];
                                        gc.config.progressive.auxrtp[i] = data.auxrtp[i];
                                        gc.config.progressive.ceil[i] = data.ceil[i];
                                    }
                                }
                                else
                                {
                                    for (uint8_t i = 0; i < game_num; ++i)
                                    {
                                        if (p->data.control.game_config_state[i] & QCOM_GAME_CONFIG_READY)
                                        {
                                            QcomGameData &gc = p->data.games[i];
                                            for (uint8_t j = 0; j < pnum; ++j)
                                            {
                                                gc.config.progressive.sup[j] = data.sup[j];
                                                gc.config.progressive.pinc[j] = data.pinc[j];
                                                gc.config.progressive.auxrtp[j] = data.auxrtp[j];
                                                gc.config.progressive.ceil[j] = data.ceil[j];
                                            }
                                        }
                                    }
                                }

                                pgid = p->data.games[game].config.settings.pgid; // return the pgid anyway

                                return true;
                            }
                            else
                            {
                                ipnum = p->data.games[game].prog.pnum;
                            }
                        }
                        else
                        {
                            istate = true;
                        }

                        break;
                    }
                }
            }

            if (game >= game_num)
            {
                COMMS_LOG(boost::format("Can't set progressive configuration. GVN 0x%|04X| is not avaiable\n") % gvn, CLL_Error);
            }
            else if (istate)
            {
                COMMS_LOG(boost::format("Can't set progressive configuration for Game(GVN 0x%|04X|). Game is not configured or verified\n") % gvn, CLL_Error);
            }
            else if (ipnum)
            {
                COMMS_LOG(boost::format("Can't set progressive configuration. Invalid progressive levels num: %|| is set while %|| is expected.\n") % pnum % ipnum, CLL_Error);
            }
        }

        return false;
    }

    QcomPollPtr QcomProgressiveConfig::MakeProgConfigPoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const & data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_PCP_FC;
        
        poll->poll.Data.pcp.GVN = gvn;
        poll->poll.Data.pcp.RES = 0;
        poll->poll.Data.pcp.FLG = 0;
        poll->poll.Data.pcp.NUM = pnum;
        poll->poll.Data.pcp.SIZ = sizeof(qc_pcpretype);

        for (uint8_t i = 0; i < pnum; ++i)
        {
            qc_pcpretype &re = poll->poll.Data.pcp.re[i];

            re.PLF.bits.lev = i;
            re.PLF.bits.res = 0;
            re.SUP = data.sup[i];
            re.PINC = data.pinc[i];
            re.CEIL = data.ceil[i];
            re.AUXRTP = data.auxrtp[i];
        }

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_pcptype) - sizeof(poll->poll.Data.pcp.re) +
            poll->poll.Data.pcp.SIZ * pnum;

        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

}

