 
#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcastSeek.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomBroadcastSeek::Id() const
    {
        return QCOM_BROADCAST_SEEK_FC;
    }

    bool QcomBroadcastSeek::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

            // check the integrity of the data
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_segmbrtype)))
            {

                if (it->FindEgmData(
                    [&](QcomDataPtr const& pd) 
                    {
                        if (pd->data.control.serialMidBCD == p->Data.segmbr.SN.SER)
                            return true;
                        return false;
                    }) == nullptr)
                {
                    QcomDataPtr pd = it->AddNewEgm();

                    std::memset(&pd->data, 0, sizeof(QcomEGMData));

                    pd->data.control.serialMidBCD= p->Data.segmbr.SN.SER;
                    pd->data.control.last_control |= (QCOM_ACK_MASK | QCOM_LAMAPOLL_MASK);
                }

                return true;

            }

        }

        return false;
    }

    bool QcomBroadcastSeek::BuildSeekEGMPoll(QcomJobDataPtr &job)
    {
        //if (auto it = m_qcom.lock())
        //{
            //QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST_SEEK);

            QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

            std::memset(poll.get(), 0, sizeof(QcomPoll));

            poll->poll.DLL.PollAddress = QCOM_BROADCAST_SEEK_FC;
            poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE;
            // ACK is not used in broadcast poll
            poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
            poll->poll.DLL.FunctionCode = QCOM_BROADCAST_SEEK_FC;
            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
            poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

            job->SetBroadcast(poll);

            return true;

            //it->AddJob(job);
        //}
    }

}


