#include "Core.hpp"
#include "Utils.hpp"
#include "BaseInteger.hpp"

#include "Qcom/QcomProgressiveMeters.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomProgressiveMeters::RespId() const
    {
        return QCOM_PMR_FC;
    }

    bool QcomProgressiveMeters::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_pmrtype2) - sizeof(p->Data.pmr2.re) + p->Data.pmr2.NUM * p->Data.pmr2.SIZ))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint16_t igvn = 0;
                uint16_t ipgid = 0;
                uint8_t pnum = 0;
                uint8_t ilevel_num = 0;
                uint8_t ilevel[QCOM_REMAX_PMR];
                uint8_t itype_num = 0;
                uint8_t itype[QCOM_REMAX_PMR];
                uint8_t itype_level[QCOM_REMAX_PMR];

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    uint8_t game = 0;
                    for (; game < pd->data.config.games_num; ++game)
                    {
                        if (pd->data.games[game].gvn == p->Data.pmr2.GVN)
                        {
                            break;
                        }
                    }

                    if (game != pd->data.config.games_num)
                    {
                        if (pd->data.games[game].config.settings.pgid == p->Data.pmr2.PGID)
                        {
                            pd->data.games[game].prog.pamt = p->Data.pmr2.PAMT;

                            for (uint8_t i = 0; i < p->Data.pmr2.NUM; ++i)
                            {
                                qc_pmrretype2 * pmr = (qc_pmrretype2*)((uint8_t*)p->Data.pmr2.re +
                                    i * p->Data.pmr2.SIZ);

                                if (pmr->PLEV.bits.level < pd->data.games[game].prog.pnum)
                                {
                                    if (pmr->PLEV.bits.LP == pd->data.games[game].config.progressive.flag_p[pmr->PLEV.bits.level])
                                    {
                                        QcomProgressiveLevelData & plevels = pd->data.games[game].prog.levels;

                                        plevels.camt[pmr->PLEV.bits.level] = pmr->CAMT;
                                        plevels.hits[pmr->PLEV.bits.level] = pmr->HITS;
                                        plevels.wins[pmr->PLEV.bits.level] = pmr->WINS;
                                        plevels.hrate[pmr->PLEV.bits.level] = pmr->HRATE;
                                    }
                                    else
                                    {
                                        itype_level[itype_num] = pmr->PLEV.bits.level;
                                        itype[itype_num++] = pd->data.games[game].config.progressive.flag_p[pmr->PLEV.bits.level];
                                    }
                                }
                                else
                                {
                                    pnum = pd->data.games[game].prog.pnum;
                                    ilevel[ilevel_num++] = pmr->PLEV.bits.level;
                                }
                            }
                        }
                        else
                        {
                            ipgid = pd->data.games[game].config.settings.pgid;
                        }
                    }
                    else
                    {
                        igvn = p->Data.pmr2.GVN;
                    }
                }

                if (igvn)
                {
                    COMMS_LOG(boost::format("Progressive Meters Response received, received GVN 0x%|04X| is invalid\n") % igvn, CLL_Error);
                }
                else if (ipgid)
                {
                    COMMS_LOG(boost::format("Progressive Meters Response received, PGID %|| doesn't math Game(GVN 0x%|04X|) PGID %||\n") %
                        p->Data.pmr2.PGID % p->Data.pmr2.GVN % ipgid, CLL_Error);
                }
                else if (ilevel_num)
                {
                    COMMS_START_LOG_BLOCK();
                    COMMS_LOG_BLOCK(boost::format("Progressive Meters Response, invalid progressive level ID received. Level Numbers:%||\n") %
                        static_cast<uint32_t>(pnum), CLL_Error);
                    for (uint8_t i = 0; i < ilevel_num; ++i)
                    {
                        COMMS_LOG_BLOCK(boost::format("Level ID: %|| received\n") % static_cast<uint32_t>(ilevel[i]) , CLL_Error);
                    }
                    COMMS_END_LOG_BLOCK();
                }
                else if (itype_num)
                {
                    COMMS_START_LOG_BLOCK();
                    COMMS_LOG_BLOCK("Progressive Meters Response, invalid progressive level type received\n", CLL_Error);
                    for (uint8_t i = 0; i < itype_num; ++i)
                    {
                        COMMS_LOG_BLOCK(boost::format("Level %|| should be %||\n") %
                            static_cast<uint32_t>(itype_level[i]) %
                            (itype[i] ? "LP" : "SAP") , CLL_Error);
                    }
                    COMMS_END_LOG_BLOCK();
                }
                else
                {
                    COMMS_LOG("Progressive Meters Response received, data updated\n", CLL_Info);
                    return true;
                }

            }
            else
            {
                COMMS_LOG(boost::format("Progressive Meters Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }
}


