#include "core/core.hpp"
#include "core/core_utils.hpp"

#include "comms/qcom/qcom_broadcast.hpp"
#include "comms/qcom/qcom_egm_config.hpp"
#include "comms/qcom/qogr/qogr_crc.h"

namespace sg {

    uint8_t QcomEgmConfiguration::Id() const
    {
        return QCOM_EGMCP_FC;
    }

    bool QcomEgmConfiguration::Parse(uint8_t buf[], int length)
    {
        CORE_UNREF_PARAM(buf);
        CORE_UNREF_PARAM(length);

        return true;

    }

    QcomPollPtr QcomEgmConfiguration::MakeEGMConfigPoll(uint8_t poll_address, uint8_t last_control, uint32_t ser, uint8_t jur, uint32_t den, uint32_t tok, uint32_t maxden, uint16_t minrtp, uint16_t maxrtp, uint16_t maxsd, uint16_t maxlines, uint32_t maxbet, uint32_t maxnpwin, uint32_t maxpwin, uint32_t maxect)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));


        // ref Qcom1.6-15.4.2
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmcptype2);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMCP_FC;
        poll->poll.Data.egmcp2.SN.SER = ser;
        poll->poll.Data.egmcp2.JUR = jur;
        poll->poll.Data.egmcp2.DEN2 = den;
        poll->poll.Data.egmcp2.TOK2 = tok;
        poll->poll.Data.egmcp2.MAXDEN = maxden;
        poll->poll.Data.egmcp2.MINRTP = minrtp;
        poll->poll.Data.egmcp2.MAXRTP = maxrtp;
        poll->poll.Data.egmcp2.MAXSD  = maxsd;
        poll->poll.Data.egmcp2.MAXLINES = maxlines;
        poll->poll.Data.egmcp2.MAXBET = maxbet;
        poll->poll.Data.egmcp2.MAXNPWIN = maxnpwin;
        poll->poll.Data.egmcp2.MAXPWIN = maxpwin;
        poll->poll.Data.egmcp2.MAXECT = maxect;
        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomEgmConfiguration::BuildEGMConfigPoll(std::vector<QcomEGMConfigCustomData> const& data)
    {
        if (CORE_AUTO(it, m_qcom.lock()))
        {
            bool pac_sent = false;

            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

            typedef CORE_DECLTYPE(data) CustomDataTypes;
            CORE_FOREACH(CustomDataTypes::const_reference d, data)
            {
                QcomDataPtr p = it->GetEgmData(d.egm);

                if (p)
                {
                    unique_lock<shared_mutex> lock(p->locker);

                    if (!pac_sent && p->data.poll_address == 0)
                    {
                        QcomBroadcastPtr pb = static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                        if (pb)
                        {
                            // TODO : it's better broadcast supply a function that
                            // it can just config specified egm poll address instead of all of them
                            pb->BuildPollAddressPoll();
                        }

                        pac_sent = true;
                    }

                    QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

                    std::memset(poll.get(), 0, sizeof(QcomPoll));

                    // ref Qcom1.6-15.4.2
                    poll->poll.Data.egmcp2.SN.SER = p->data.serialMidBCD;

                    if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                        p->data.last_control ^= (QCOM_ACK_MASK);

                    job->AddPoll(MakeEGMConfigPoll(d.egm, p->data.last_control, p->data.serialMidBCD, d.data.jur, d.data.den, d.data.tok, d.data.maxden, d.data.minrtp, d.data.maxrtp, d.data.maxsd, d.data.maxlines, d.data.maxbet, d.data.maxnpwin, d.data.maxpwin, d.data.maxect));

                    // store the data to egm data
                    p->data.resp_funcode = QCOM_NO_RESPONSE;
                    p->data.egm_config.jur = d.data.jur;
                    p->data.egm_config.den = d.data.den;
                    p->data.egm_config.tok = d.data.tok;
                    p->data.egm_config.maxden = d.data.maxden;
                    p->data.egm_config.minrtp = d.data.minrtp;
                    p->data.egm_config.maxrtp = d.data.maxrtp;
                    p->data.egm_config.maxsd = d.data.maxsd;
                    p->data.egm_config.maxlines = d.data.maxlines;
                    p->data.egm_config.maxbet = d.data.maxbet;
                    p->data.egm_config.maxnpwin = d.data.maxnpwin;
                    p->data.egm_config.maxpwin = d.data.maxpwin;
                    p->data.egm_config.maxect  = d.data.maxect;

                }
            }

            it->AddJob(job);

        }
    }

    void QcomEgmConfiguration::BuildEGMConfigPoll(uint8_t poll_address, uint8_t jur, uint32_t den, uint32_t tok, uint32_t maxden, uint16_t minrtp, uint16_t maxrtp, uint16_t maxsd, uint16_t maxlines, uint32_t maxbet, uint32_t maxnpwin, uint32_t maxpwin, uint32_t maxect)
    {
        if (CORE_AUTO(it, m_qcom.lock()))
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

                unique_lock<shared_mutex> lock(p->locker);

                if (p->data.poll_address == 0)
                {
                    QcomBroadcastPtr pb = static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                    if (pb)
                    {
                        pb->BuildPollAddressPoll();
                    }
                }

                if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                    p->data.last_control ^= (QCOM_ACK_MASK);

                job->AddPoll(MakeEGMConfigPoll(poll_address, p->data.last_control, p->data.serialMidBCD, jur, den, tok, maxden, minrtp, maxrtp, maxsd, maxlines, maxbet, maxnpwin, maxpwin, maxect));


                p->data.resp_funcode = QCOM_NO_RESPONSE;
                p->data.egm_config.jur = jur;
                p->data.egm_config.den = den;
                p->data.egm_config.tok = tok;
                p->data.egm_config.maxden = maxden;
                p->data.egm_config.minrtp = minrtp;
                p->data.egm_config.maxrtp = maxrtp;
                p->data.egm_config.maxsd = maxsd;
                p->data.egm_config.maxlines = maxlines;
                p->data.egm_config.maxbet = maxbet;
                p->data.egm_config.maxnpwin = maxnpwin;
                p->data.egm_config.maxpwin = maxpwin;
                p->data.egm_config.maxect  = maxect;

                it->AddJob(job);
            }

        }
    }

}
