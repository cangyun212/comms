#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomGameConfigReq.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg {

    namespace {

        bool FindEGMDataBySER(uint32_t ser, QcomDataPtr p)
        {
			std::unique_lock<std::mutex> lock(p->locker);
            if (p->data.serialMidBCD == ser)
            {
                return true;
            }

            return false;
        }
    }

    bool QcomGameConfigurationRequest::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_egmgcrtype)))
            {
                QcomDataPtr pd = it->FindEgmData(std::bind(&FindEGMDataBySER, p->DLL.PollAddress, std::placeholders::_1));

                if (pd)
                {
					std::unique_lock<std::mutex> lock(pd->locker);

                    //pd->data.last_control ^= (QCOM_ACK_MASK);

                    // TODO : the following fields must not change
                    // once reported by the egm in the first egm configuration response
                    // except NUMG, DEN, TOK & CRC
                    pd->data.last_gvn = p->Data.egmgcr.GVN;

                    return true;
                }
            }
        }

        return false;

    }

    uint8_t QcomGameConfigurationRequest::Id() const
    {
       return 0;
    }



}
