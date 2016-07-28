#include "Core.hpp"

#include "Utils.hpp"

#include "Qcom/QcomSystemLockupRequest.hpp"
#include "Qcom/qogr/qogr_crc.h"
#include "Qcom/QcomInline.h"


namespace sg
{
    uint8_t QcomSysLockupReq::Id() const
    {
        return QCOM_SALRP_FC;
    }

    bool QcomSysLockupReq::BuildSysLockupReqPoll(QcomJobDataPtr & job, uint8_t poll_address, QcomSysLockupRequestData const & data)
    {
        if (_QComIsPrint(data.text, data.len))
        {
            if (auto it = m_qcom.lock())
            {
                QcomDataPtr p = it->GetEgmData(poll_address);
                if (p)
                {
                    std::unique_lock<std::mutex> lock(p->locker);

                    job->AddPoll(this->MakeSysLockupReqPoll(poll_address, p->data.control.last_control, data));

                    return true;
                }
            }
        }
        else
        {
            COMMS_LOG("System Lockup poll's text contains non-printable character\n", CLL_Error);
        }

        return false;
    }

    QcomPollPtr QcomSysLockupReq::MakeSysLockupReqPoll(uint8_t poll_address, uint8_t last_control, QcomSysLockupRequestData const & data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_SALRP_FC;

        poll->poll.Data.salrp.FLG.bits.res = 0;
        poll->poll.Data.salrp.FLG.bits.resetkeydisable = data.no_resetkey;
        poll->poll.Data.salrp.FLG.bits.continu = data.continue_style;
        poll->poll.Data.salrp.FLG.bits.question = data.question_style;
        poll->poll.Data.salrp.FLG.bits.lamptest = data.lamp_test;
        poll->poll.Data.salrp.FLG.bits.fanfare = data.fanfare;

        poll->poll.Data.salrp.LEN = data.len;
        if (data.len)
        {
            std::memcpy(poll->poll.Data.salrp.TEXT, data.text, data.len);
        }

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_salrptype) - sizeof(poll->poll.Data.salrp.TEXT) + data.len;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}





