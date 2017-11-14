#include "Core.hpp"
#include "Utils.hpp"
#include "BaseInteger.hpp"

#include "Qcom/QcomMultiGameVarMeters.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomMultiGameVarMeters::RespId() const
    {
        return QCOM_MGVMR_FC;
    }

    bool QcomMultiGameVarMeters::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_mgvmrtype)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint16_t igvn = 0;
                uint8_t ivar = 0;
                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    uint8_t game = 0;
                    for (; game < pd->data.config.games_num; ++game)
                    {
                        if (pd->data.games[game].gvn == p->Data.mgvmr.GVN)
                        {
                            break;
                        }
                    }

                    if (game != pd->data.config.games_num)
                    {
                        if (pd->data.games[game].config.settings.var == p->Data.mgvmr.VAR)
                        {
                            pd->data.games[game].mgvm.str = p->Data.mgvmr.meters.meter.STR;
                            pd->data.games[game].mgvm.turn = p->Data.mgvmr.meters.meter.TURN;
                            pd->data.games[game].mgvm.win = p->Data.mgvmr.meters.meter.WIN;
                            pd->data.games[game].mgvm.pwin = p->Data.mgvmr.meters.meter.PWIN;
                            pd->data.games[game].mgvm.gwin = p->Data.mgvmr.meters.meter.GWON;
                        }
                        else
                        {
                            ivar = pd->data.games[game].config.settings.var;
                        }
                    }
                    else
                    {
                        igvn = p->Data.mgvmr.GVN;
                    }
                }

                if (igvn)
                {
                    COMMS_LOG(boost::format("Multi-Game/Var Response received, received GVN 0x%|04X| is invalid\n") % igvn, CLL_Error);
                }
                else if (ivar)
                {
                    COMMS_LOG(boost::format("Multi-Game/Var Response received, received VAR %|02d| is not current VAR %|02d| for Game(GVN 0x%|04X|)\n") %
                        static_cast<uint32_t>(p->Data.mgvmr.VAR) % static_cast<uint32_t>(ivar) % p->Data.mgvmr.GVN, CLL_Error);
                }
                else
                {
                    COMMS_LOG("Multi-Game/Var Response received, data updated\n", CLL_Info);
                    return true;
                }

            }
            else
            {
                COMMS_LOG(boost::format("Multi-Game/Var Response received, data length %|| is not valid.\n") % 
                    static_cast<uint32_t>(p->DLL.Length), CLL_Error);
            }
        }

        return false;
    }
}


