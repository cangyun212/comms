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

    void QcomEgmParameters::BuildEgmParametersPoll(std::vector<QcomEgmParametersCustomData> const& data)
    {
        if (auto it = m_qcom.lock())
        {
            bool pac_sent = false;

            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);
            for(auto const& d : data)
            {
                QcomDataPtr p = it->GetEgmData(d.egm);

                if (p)
                {
					std::unique_lock<std::mutex> lock(p->locker);

                    if (!pac_sent && p->data.poll_address == 0)
                    {
                        QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                        if (pb)
                        {
                            // TODO : it's better broadcast supply a function that
                            // it can just config specified egm poll address instead of all of them
                            pb->BuildPollAddressPoll();
                        }

                        pac_sent = true;
                    }

                    if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                        p->data.last_control ^= (QCOM_ACK_MASK);

                    job->AddPoll(this->MakeEgmParametersPoll(d.egm, 
								p->data.last_control, 
								d.params.reserve, 
								d.params.autoplay, 
								d.params.crlimitmode, 
								d.params.opr, 
								d.params.lwin, 
								d.params.crlimit, 
								d.params.dumax, 
								d.params.dulimit, 
								d.params.tzadj, 
								d.params.pwrtime, 
								d.params.pid, 
								d.params.eodt, 
								d.params.npwinp, 
								d.params.sapwinp));

                    // store the data to game data
                    p->data.resp_funcode = QCOM_NO_RESPONSE;
                    p->data.egm_params.reserve = d.params.reserve;
                    p->data.egm_params.autoplay = d.params.autoplay;
                    p->data.egm_params.crlimitmode = d.params.crlimitmode;
                    p->data.egm_params.opr = d.params.opr;
                    p->data.egm_params.lwin = d.params.lwin;
                    p->data.egm_params.crlimit = d.params.crlimit;
                    p->data.egm_params.dumax = d.params.dumax;
                    p->data.egm_params.dulimit = d.params.dulimit;
                    p->data.egm_params.tzadj = d.params.tzadj;
                    p->data.egm_params.pwrtime = d.params.pwrtime;
                    p->data.egm_params.pid = d.params.pid;
                    p->data.egm_params.eodt = d.params.eodt;
                    p->data.egm_params.npwinp = d.params.npwinp;
                    p->data.egm_params.sapwinp = d.params.sapwinp;

                }
            }

            it->AddJob(job);

        }
    }

    void QcomEgmParameters::BuildEgmParametersPoll(uint8_t poll_address, uint8_t reserve, uint8_t autoplay, uint8_t crlimitmode, uint8_t opr,
                                                   uint32_t lwin, uint32_t crlimit, uint8_t dumax, uint32_t dulimit, uint16_t tzadj, uint32_t pwrtime,
                                                   uint8_t pid, uint16_t eodt, uint32_t npwinp, uint32_t sapwinp)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

				std::unique_lock<std::mutex> lock(p->locker);

                if (p->data.poll_address == 0)
                {
                    QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                    if (pb)
                    {
                        pb->BuildPollAddressPoll();
                    }
                }


                if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                    p->data.last_control ^= (QCOM_ACK_MASK);

                job->AddPoll(this->MakeEgmParametersPoll(poll_address, 
							p->data.last_control, 
							reserve, 
							autoplay, 
							crlimitmode, 
							opr, 
							lwin, 
							crlimit, 
							dumax, 
							dulimit, 
							tzadj, 
							pwrtime, 
							pid, 
							eodt, 
							npwinp, 
							sapwinp));

                p->data.resp_funcode = QCOM_NO_RESPONSE;
                p->data.egm_params.reserve = reserve;
                p->data.egm_params.autoplay = autoplay;
                p->data.egm_params.crlimitmode = crlimitmode;
                p->data.egm_params.opr = opr;
                p->data.egm_params.lwin = lwin;
                p->data.egm_params.crlimit = crlimit;
                p->data.egm_params.dumax = dumax;
                p->data.egm_params.dulimit = dulimit;
                p->data.egm_params.tzadj = tzadj;
                p->data.egm_params.pwrtime = pwrtime;
                p->data.egm_params.pid = pid;
                p->data.egm_params.eodt = eodt;
                p->data.egm_params.npwinp = npwinp;
                p->data.egm_params.sapwinp = sapwinp;
                it->AddJob(job);
            }
        }
    }

    QcomPollPtr QcomEgmParameters::MakeEgmParametersPoll(uint8_t poll_address, uint8_t last_control, uint8_t reserve, uint8_t autoplay, uint8_t crlimitmode, uint8_t opr,
                                                         uint32_t lwin, uint32_t crlimit, uint8_t dumax, uint32_t dulimit, uint16_t tzadj, uint32_t pwrtime,
                                                         uint8_t pid, uint16_t eodt, uint32_t npwinp, uint32_t sapwinp)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmpptype2);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMPP_FC;

        poll->poll.Data.egmpp2.FLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmpp2.FLG.bits.resfeat = reserve;
        poll->poll.Data.egmpp2.FLG.bits.autoplay = autoplay;
        poll->poll.Data.egmpp2.FLG.bits.crlimitmode = crlimitmode;
        poll->poll.Data.egmpp2.OPR = opr;
        poll->poll.Data.egmpp2.LWIN = lwin;
        poll->poll.Data.egmpp2.CRLIMIT = crlimit;
        poll->poll.Data.egmpp2.DUMAX = dumax;
        poll->poll.Data.egmpp2.DULIMIT = dulimit;
        poll->poll.Data.egmpp2.TZADJ = tzadj;
        poll->poll.Data.egmpp2.PWRTIME = pwrtime;
        poll->poll.Data.egmpp2.PID = pid;
        poll->poll.Data.egmpp2.EODT = eodt;
        poll->poll.Data.egmpp2.NPWINP = npwinp;
        poll->poll.Data.egmpp2.SAPWINP = sapwinp;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
