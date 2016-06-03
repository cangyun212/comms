#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomEgmParameters.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg {

    uint8_t QcomEgmParameters::Id() const
    {
        return QCOM_EGMPP_FC;
    }

    bool QcomEgmParameters::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(buf);
        SG_UNREF_PARAM(length);

        return true;

    }

    void QcomEgmParameters::BuildEgmParametersPoll(uint8_t poll_address, QcomEGMParametersData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeEgmParametersPoll(poll_address, p->data.control.last_control, data));

                p->data.param = data;

                it->AddJob(job);
            }
        }
    }

    QcomPollPtr QcomEgmParameters::MakeEgmParametersPoll(uint8_t poll_address, uint8_t last_control, QcomEGMParametersData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmpptype2);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMPP_FC;

        poll->poll.Data.egmpp2.FLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmpp2.FLG.bits.resfeat = data.reserve;
        poll->poll.Data.egmpp2.FLG.bits.autoplay = data.auto_play;
        poll->poll.Data.egmpp2.FLG.bits.crlimitmode = data.crlimit_mode;
        poll->poll.Data.egmpp2.OPR = data.opr;
        poll->poll.Data.egmpp2.LWIN = data.lwin;
        poll->poll.Data.egmpp2.CRLIMIT = data.crlimit;
        poll->poll.Data.egmpp2.DUMAX = data.dumax;
        poll->poll.Data.egmpp2.DULIMIT = data.dulimit;
        poll->poll.Data.egmpp2.TZADJ = data.tzadj;
        poll->poll.Data.egmpp2.PWRTIME = data.pwrtime;
        poll->poll.Data.egmpp2.PID = data.pid;
        poll->poll.Data.egmpp2.EODT = data.eodt;
        poll->poll.Data.egmpp2.NPWINP = data.npwinp;
        poll->poll.Data.egmpp2.SAPWINP = data.sapwinp;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
