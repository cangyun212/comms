#include "Core.hpp"
#include "Utils.hpp"
#include "BaseInteger.hpp"

#include "Qcom/QcomPlayerChoiceMeters.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomPlayerChoiceMeters::RespId() const
    {
        return QCOM_PCMR_FC;
    }

    bool QcomPlayerChoiceMeters::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_pcmrtype) - sizeof(p->Data.pcmr.PMET) + p->Data.pcmr.NUM * p->Data.pcmr.SIZ))
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
                        if (pd->data.games[game].gvn == p->Data.pcmr.GVN)
                        {
                            break;
                        }
                    }

                    if (game != pd->data.config.games_num)
                    {
                        pd->data.games[game].pcm.num = p->Data.pcmr.NUM;
                        for (uint8_t i = 0; i < p->Data.pcmr.NUM; ++i)
                        {
                            uint32_t *pcmet = (uint32_t*)((uint8_t*)(p->Data.pcmr.PMET) + i * p->Data.pcmr.SIZ);
                            pd->data.games[game].pcm.pcmet[i] = *pcmet;
                        }
                    }
                    else
                    {
                        igvn = p->Data.pcmr.GVN;
                    }
                }

                if (igvn)
                {
                    COMMS_LOG(boost::format("Player Choice Meters Response received, received GVN 0x%|04X| is invalid\n") % igvn, CLL_Error);
                }
                else
                {
                    COMMS_LOG("Player Choice Meters Response received, data updated.\n", CLL_Info);
                    return true;
                }

            }
            else
            {
                COMMS_LOG(boost::format("Player Choice Meters Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }
}


