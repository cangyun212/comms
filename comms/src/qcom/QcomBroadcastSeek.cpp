 
#include "core/core.hpp"
#include "core/core_utils.hpp"

#include "comms/qcom/qcom_broadcast_seek.hpp"
#include "comms/qcom/qogr/qogr_crc.h"

namespace sg {

    namespace {

        bool CheckSerialMidBCD(uint32_t ser, QcomDataPtr p)
        {
            // unique_lock lock(p->locker);

            if (p->data.serialMidBCD == ser)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }


    uint8_t QcomBroadcastSeek::Id() const
    {
        return QCOM_BROADCAST_SEEK_FC;
    }

    bool QcomBroadcastSeek::Parse(uint8_t buf[], int length)
    {
        CORE_UNREF_PARAM(length);

        if (CORE_AUTO(it, m_qcom.lock()))
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            // check the integrity of the data
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_segmbrtype)))
            {

                if (it->FindEgmData(
                            bind(&CheckSerialMidBCD,
                                 p->Data.segmbr.SN.SER,
                                 placeholders::_1)) == nullptr)
                {
                    QcomDataPtr pd = it->AddNewEgm();

                    std::memset(&pd->data, 0, sizeof(EGMData));

                    pd->data.serialMidBCD = p->Data.segmbr.SN.SER;
                    pd->data.last_control |= (QCOM_ACK_MASK | QCOM_LAMAPOLL_MASK);
                }

                return true;

            }

        }

        return false;
    }

    void QcomBroadcastSeek::BuildSeekEGMPoll()
    {
        if (CORE_AUTO(it, m_qcom.lock()))
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST_SEEK);

            QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

            std::memset(poll.get(), 0, sizeof(QcomPoll));

            poll->poll.DLL.PollAddress = QCOM_BROADCAST_SEEK_FC;
            poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE;
            // ACK is not used in broadcast poll
            poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
            poll->poll.DLL.FunctionCode = QCOM_BROADCAST_SEEK_FC;
            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
            poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

            job->AddBroadcast(poll);

            it->AddJob(job);
        }
    }

}


