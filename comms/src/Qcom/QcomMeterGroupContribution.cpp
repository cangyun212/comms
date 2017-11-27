#include "Core.hpp"
#include "Utils.hpp"
#include "BaseInteger.hpp"

#include "Qcom/QcomMeterGroupContribution.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomMeterGroupContribution::RespId() const
    {
        return QCOM_MGCR_FC;
    }

    bool QcomMeterGroupContribution::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            size_t size = p->Data.mgcr.FLG.bits.num * sizeof(*(&(p->Data.mgcr.re[0])));
            if (p->Data.mgcr.FLG.bits.LP)
                size += sizeof(qc_progrconttype);

            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_mgcrtype) - sizeof(p->Data.mgcr.re) + size))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                bool igvn = false;
                uint8_t imgid = 0xFF;
                uint16_t ipgid = 0;
                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    //pd->data.mgc.lgvn = p->Data.mgcr.LGVN;
                    uint8_t game = 0;
                    for (; game < pd->data.config.games_num; ++game)
                    {
                        if (pd->data.games[game].gvn == p->Data.mgcr.LGVN)
                        {
                            break;
                        }
                    }

                    if (game < pd->data.config.games_num)
                    {
                        for (uint8_t i = 0; i < p->Data.mgcr.FLG.bits.num; ++i)
                        {
                            qc_mgcrretype *pmg = (qc_mgcrretype*)((uint8_t*)(p->Data.mgcr.re) + i * sizeof(qc_mgcrretype));

                            uint8_t gid = 0;
                            uint8_t mid = 0;
                            if (pmg->MGID <= 0x0F)
                            {
                                gid = 0;
                                mid = pmg->MGID;
                            }
                            else if (pmg->MGID >= 0x10 && pmg->MGID <= 0x1F)
                            {
                                gid = 1;
                                mid = pmg->MGID - 0x10;
                            }
                            else if (pmg->MGID >= 0x20 && pmg->MGID <= 0x2F)
                            {
                                gid = 2;
                                mid = pmg->MGID - 0x20;
                            }
                            else
                            {
                                imgid = pmg->MGID;
                                break;
                            }
                            
                            pd->data.games[game].mgc.groups[gid].met[mid] = pmg->MET;
                        }

                        if (p->Data.mgcr.FLG.bits.LP)
                        {
                            qc_progrconttype *plpc = ((qc_progrconttype *)(&p->Data.mgcr.re[p->Data.mgcr.FLG.bits.num + 1]));

                            if (pd->data.games[game].config.settings.pgid == plpc->PGID)
                            {
                                pd->data.games[game].mgc.pamt = plpc->CAMT;
                            }
                            else
                            {
                                ipgid = pd->data.games[game].config.settings.pgid;
                            }
                        }
                    }
                    else
                    {
                        igvn = true;
                    }
                }

                if (igvn)
                {
                    COMMS_LOG(boost::format("Meter Group/Contribution Resonse received, invalid GVN 0x%|04X|\n") %
                        p->Data.mgcr.LGVN, CLL_Error);

                } else if (imgid != 0xFF)
                {
                    COMMS_LOG(boost::format("Meter Group/Contribution Response received, received MGID 0x%|02X| is invalid\n") % 
                        static_cast<uint32_t>(imgid), CLL_Error);
                }
                else if (ipgid != 0)
                {

                    qc_progrconttype *plpc = ((qc_progrconttype *)(&p->Data.mgcr.re[p->Data.mgcr.FLG.bits.num + 1]));
                    COMMS_LOG(boost::format("Meter Group/Contribution Response received, PGID 0x%|04x| is not valid for Game(GVN 0x%|04X|), 0x%|04X| is expected\n") %
                        plpc->PGID % p->Data.mgcr.LGVN % ipgid, CLL_Error);
                }
                else
                {
                    COMMS_LOG("Meter Group/Contribution Response received, data updated.\n", CLL_Info);
                    return true;
                }
            }
            else
            {
                COMMS_LOG(boost::format("Meter Group/Contribution Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }
}


