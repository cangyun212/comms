#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomEgmConfig.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{
    uint8_t QcomEgmConfiguration::Id() const
    {
        return QCOM_EGMCP_FC;
    }

    bool QcomEgmConfiguration::BuildEGMConfigPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomEGMConfigPollData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(
                    this->MakeEGMConfigPoll(poll_address, p->data.control.last_control, p->data.control.serialMidBCD, data));

                p->data.custom = data;

                p->data.control.egm_config_state |= QCOM_EGM_CONFIG_SET;

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomEgmConfiguration::MakeEGMConfigPoll(uint8_t poll_address, uint8_t last_control, uint32_t ser, 
        QcomEGMConfigPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmcptype2);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMCP_FC;
        poll->poll.Data.egmcp2.SN.SER = ser;
        poll->poll.Data.egmcp2.JUR = data.jur;
        poll->poll.Data.egmcp2.DEN2 = data.den;
        poll->poll.Data.egmcp2.TOK2 = data.tok;
        poll->poll.Data.egmcp2.MAXDEN = data.maxden;
        poll->poll.Data.egmcp2.MINRTP = data.minrtp;
        poll->poll.Data.egmcp2.MAXRTP = data.maxrtp;
        poll->poll.Data.egmcp2.MAXSD  = data.maxsd;
        poll->poll.Data.egmcp2.MAXLINES = data.maxlines;
        poll->poll.Data.egmcp2.MAXBET = data.maxbet;
        poll->poll.Data.egmcp2.MAXNPWIN = data.maxnpwin;
        poll->poll.Data.egmcp2.MAXPWIN = data.maxpwin;
        poll->poll.Data.egmcp2.MAXECT = data.maxect;
        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

}
