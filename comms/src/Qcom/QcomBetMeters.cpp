#include "Core.hpp"
#include "Utils.hpp"
#include "BaseInteger.hpp"

#include "Qcom/QcomBetMeters.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomBetMeters::RespId() const
    {
        return QCOM_BMR_FC;
    }

    bool QcomBetMeters::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            uint32_t num = p->Data.bmr.GFA * p->Data.bmr.GFB;
            if (num > QCOM_BMR_MAX_CMET)
            {
                COMMS_LOG(boost::format("Bet Meters Response, invaild GBFA %|| and GBFB %|| received\n") %
                    p->Data.bmr.GFA % p->Data.bmr.GFB, CLL_Error);
                return false;
            }

            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_pmrtype2) - sizeof(p->Data.bmr.CMET) + num * p->Data.bmr.SIZ))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint16_t igvn = 0;

                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    uint8_t game = 0;
                    for (; game < pd->data.config.games_num; ++game)
                    {
                        if (pd->data.games[game].gvn == p->Data.bmr.GVN)
                        {
                            break;
                        }
                    }

                    if (game != pd->data.config.games_num)
                    {
                        pd->data.games[game].betm.maxl = p->Data.bmr.MAXL;
                        pd->data.games[game].betm.maxb = p->Data.bmr.MAXB;
                        pd->data.games[game].betm.gbfa = p->Data.bmr.GFA;
                        pd->data.games[game].betm.gbfb = p->Data.bmr.GFB;

                        for (uint8_t i = 0; i < num; ++i)
                        {
                            uint32_t *pcmet = (uint32_t*)((uint8_t*)(p->Data.bmr.CMET) + i * p->Data.bmr.SIZ);
                            pd->data.games[game].betm.cmet[i] = *pcmet;
                        }
                    }
                    else
                    {
                        igvn = p->Data.bmr.GVN;
                    }
                }

                if (igvn)
                {
                    COMMS_LOG(boost::format("Bet Meters Response received, received GVN 0x%|04X| is invalid\n") % igvn, CLL_Error);
                }
                else
                {
                    COMMS_LOG("Bet Meters Response received, data updated.\n", CLL_Info);
                    return true;
                }

            }
            else
            {
                COMMS_LOG(boost::format("Bet Meters Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }
}


