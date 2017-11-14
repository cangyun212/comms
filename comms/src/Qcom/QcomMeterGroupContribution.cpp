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

            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_pmrtype2) - sizeof(p->Data.mgcr.re) + size))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                uint8_t imgid = 0xFF;
                if (pd)
                {
                    std::unique_lock<std::mutex> lock(pd->locker);

                    pd->data.control.last_control ^= (QCOM_ACK_MASK);

                    pd->data.mgc.lgvn = p->Data.mgcr.LGVN;

                    for (uint8_t i = 0; i < p->Data.mgcr.FLG.bits.num; ++i)
                    {
                        qc_mgcrretype *pmg = (qc_mgcrretype*)((uint8_t*)(p->Data.mgcr.re) + i * sizeof(qc_mgcrretype));

                        uint8_t gid = 0;
                        uint8_t mid = 0;
                        if (pmg->MGID >= 0x00 && pmg->MGID <= 0x0F)
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

                        pd->data.mgc.groups[gid].met[mid] = pmg->MET;
                    }
                }

                if (imgid != 0xFF)
                {
                    COMMS_LOG(boost::format("Meter Group/Contribution Response received, received MGID 0x%|02X| is invalid\n") % 
                        static_cast<uint32_t>(imgid), CLL_Error);
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


