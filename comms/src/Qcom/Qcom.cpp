#include "Core.hpp"

#include <iostream>

#include "Utils.hpp"

#include "Qcom/Qcom.hpp"
#include "Qcom/qogr/qogr_crc.h"
#include "Qcom/QcomBroadcastSeek.hpp"
#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomEgmConfigReq.hpp"
#include "Qcom/QcomEgmConfig.hpp"
#include "Qcom/QcomGeneralStatus.hpp"
#include "Qcom/QcomGameConfig.hpp"
#include "Qcom/QcomGameConfigReq.hpp"
#include "Qcom/QcomGameConfigChange.hpp"
#include "Qcom/QcomEgmParameters.hpp"
#include "Qcom/QcomPurgeEvents.hpp"
#include "Qcom/QcomEvent.hpp"
#include "Qcom/QcomProgressiveConfig.hpp"
#include "Qcom/QcomExtJPInfo.hpp"
#include "Qcom/QcomProgramHashRequest.hpp"
#include "Qcom/QcomSystemLockupRequest.hpp"
#include "Qcom/QcomCashTicketOutRequestAck.hpp"
#include "Qcom/QcomCashTicketInRequestAck.hpp"
#include "Qcom/QcomCashOutRequest.hpp"
#include "Qcom/QcomEgmGeneralMaintenance.hpp"
#include "Qcom/QcomRequestAllLoggedEvents.hpp"
#include "Qcom/QcomNoteAcceptorMaintenance.hpp"
#include "Qcom/QcomHTPMaintenance.hpp"
#include "Qcom/QcomLPAwardAck.hpp"
#include "Qcom/QcomGeneralReset.hpp"
#include "Qcom/QcomSPAM.hpp"
#include "Qcom/QcomTowerLightMaintenance.hpp"
#include "Qcom/QcomProgressiveMeters.hpp"
#include "Qcom/QcomBetMeters.hpp"
#include "Qcom/QcomMultiGameVarMeters.hpp"
#include "Qcom/QcomPlayerChoiceMeters.hpp"
#include "Qcom/QcomMeterGroupContribution.hpp"
#include "Qcom/QcomECTToEGM.hpp"
#include "Qcom/QcomECTFromEGMRequest.hpp"
#include "Qcom/QcomECTLockupReset.hpp"

// Typically 32, max 250
#define SG_QCOM_MAX_EGM_NUM 32

#define SG_QCOM_TRT         5
#define SG_QCOM_TPC         245

#define SG_QCOM_MAKE_JOB(job, type) QcomJobDataPtr job = nullptr;\
                                    if (!m_pending) {\
                                        job = MakeSharedPtr<QcomJobData>(type);\
                                    }\
                                    else {\
                                        job = m_pending_job;\
                                    }

#define SG_QCOM_ADD_POLL_JOB(job)   if (!m_pending) {\
                                        QcomBroadcastPtr _broadcast_handler =\
                                            std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);\
                                        if (!m_lpbroadcast) {\
                                            if (!_broadcast_handler->BuildTimeDateBroadcast(job))\
                                                return;\
                                        } else {\
                                            QcomLinkedProgressiveData _data;\
                                            if (!this->GetLPConfigData(_data))\
                                            {\
                                                COMMS_LOG("No Linked Progressive data available, fail to build broadcast message\n", CLL_Error);\
                                                return;\
                                            }\
                                            if (!_broadcast_handler->BuildLinkProgressiveCurrentAmountBroadcast(job, _data))\
                                                return;\
                                        }\
                                        this->AddJob(job);\
                                    }

#define SG_QCOM_ADD_BROADCAST_JOB(job)  if (!m_pending) {\
                                            this->AddJob(job);\
                                        }

namespace sg 
{

    QcomJobData::QcomJobData(JobType type, size_t poll_num)
        : m_type(type)
    {
        m_polls.reserve(poll_num);
    }

    size_t QcomJobData::GetPollNum() const
    {
        return m_polls.size();
    }

    QcomPollPtr QcomJobData::GetPoll(size_t index) const
    {
        return m_polls[index];
    }

    void QcomJobData::AddPoll(const QcomPollPtr &poll)
    {
        m_polls.push_back(poll);
    }

    CommsQcom::CommsQcom(const std::string &dev)
        : Comms(dev, Comms::CT_QCOM)
        , m_tpc(this->Now())
        , m_skip(true)
        , m_pending(false)
        , m_lpbroadcast(false)
    {
        m_egms.reserve(SG_QCOM_MAX_EGM_NUM);
        m_curr_lp = m_lps.begin();
    }

    CommsQcom::~CommsQcom()
    {
        this->Quit();
    }

    void CommsQcom::DoInit()
    {
        CommsPtr base= shared_from_this();
        CommsQcomPtr this_ptr = std::static_pointer_cast<CommsQcom>(base);

        CommsPacketHandlerPtr p = MakeSharedPtr<QcomBroadcastSeek>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomEgmConfigurationRequest>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomEgmConfiguration>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomGeneralStatus>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomBroadcast>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomGameConfiguration>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomGameConfigurationChange>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomEgmParameters>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomPurgeEvents>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomEvent>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));
        
        p = MakeSharedPtr<QcomProgressiveConfig>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomExtJPInfo>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomProgHashRequest>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomSysLockupReq>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomCashTicketOutRequestAck>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomCashTicketInRequestAck>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomCashTicketOutRequest>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomEgmGeneralMaintenance>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomRequestAllLoggedEvents>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomNoteAcceptorMaintenance>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomHTPMaintenance>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomLPAwardAck>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomGeneralReset>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomSPAMA>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomSPAMB>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomTowerLightMaintenance>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomProgressiveMeters>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomBetMeters>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomMultiGameVarMeters>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomPlayerChoiceMeters>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomMeterGroupContribution>(this_ptr);
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomECTToEGM>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(p->RespId(), p));

        p = MakeSharedPtr<QcomECTFromEGMRequest>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomECTLockupReset>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
    }

    CommsPacketHandlerPtr CommsQcom::GetHandler(uint8_t id) const
    {
        auto it = m_handler.find(id);
        if (it != m_handler.end())
        {
            return it->second;
        }

        return nullptr;
    }

    void CommsQcom::DoStart()
    {
        this->StartJobThread();

        for (auto & handler : m_handler)
        {
            handler.second->StartSubStageThread();
        }
    }

    void CommsQcom::DoStop()
    {
        QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_QUIT);
        this->AddJob(job);

        this->StopJobThread();

        for (auto & handler : m_handler)
        {
            handler.second->StopSubStageThread();
        }
    }

    void CommsQcom::StartJobThread()
    {
        m_worker = std::thread(std::bind(&CommsQcom::RunJob, this));
    }

    void CommsQcom::StopJobThread()
    {
        if (m_worker.joinable())
        {
            m_worker.join();
        }
    }

    void CommsQcom::AddJob(const QcomJobDataPtr &job_data)
    {
        std::unique_lock<std::mutex> lock(m_job);
        m_jobs.push_back(job_data);
        m_job_cond.notify_one();
    }

    void CommsQcom::RunJob()
    {
        while (1) {

            std::unique_lock<std::mutex> lock(m_job);
            while(!m_jobs.size())
            {
                m_job_cond.wait(lock);
            }

            QcomJobDataPtr job = m_jobs.front();
            m_jobs.pop_front();

            lock.unlock();

            cstt tpc = this->Now() - m_tpc;
            if (tpc < SG_QCOM_POLLCYCLE_TIME)
                std::this_thread::sleep_for(std::chrono::milliseconds((SG_QCOM_POLLCYCLE_TIME - tpc)));

            switch (job->GetType())
            {
            case QcomJobData::JT_POLL:
            case QcomJobData::JT_BROADCAST:
            {
                std::unique_lock<std::mutex> rsp_lock(m_response);

                size_t num = job->GetPollNum();
                for (size_t i = 0; i < num; ++i)
                {
                    QcomPollPtr poll = job->GetPoll(i);
                    this->DecoratePoll(poll);

                    m_resp_time = 0;
                    this->SendPacket(poll->data, poll->length);
                    m_resp_time = this->Now();

                    if (!m_skip)
                    {
                        if (m_response_cond.wait_for(rsp_lock, cstime(SG_JOB_TIMEOUT)) == std::cv_status::timeout)
                            m_resp_timeout = true;

                        if (m_resp_timeout)
                        {
                            COMMS_LOG("Qcom response timeout, abandon remaining polls\n", CLL_Error);
                            m_resp_timeout = false;
                            break;
                        }
                    }
                    else
                    {
                        m_skip = false;
                    }
                }

                QcomPollPtr poll = job->GetBroadcast();

                if (poll)
                    this->SendPacket(poll->data, poll->length);

                break;
            }
            case QcomJobData::JT_BROADCAST_SEEK:
            {
                std::unique_lock<std::mutex> rsp_lock(m_response);

                QcomPollPtr poll = job->GetBroadcast();

                m_resp_time = 0;
                this->SendPacket(poll->data, poll->length);
                m_resp_time = this->Now();

                if (m_response_cond.wait_for(rsp_lock, cstime(SG_JOB_TIMEOUT)) == std::cv_status::timeout)
                    m_resp_timeout = true;

                if (m_resp_timeout)
                {
                    m_resp_timeout = false;

                    std::this_thread::sleep_for(std::chrono::milliseconds(SG_RETRY_TIME));

                    m_resp_time = 0;
                    this->SendPacket(poll->data, poll->length);
                    m_resp_time = this->Now();

                    if (m_response_cond.wait_for(rsp_lock, cstime(SG_JOB_TIMEOUT)) == std::cv_status::timeout)
                        m_resp_timeout = true;

                    if (m_resp_timeout)
                    {
                        COMMS_LOG("Qcom seek broadcast response timeout\n", CLL_Error);
                        m_resp_timeout = false;
                        break;
                    }
                }

                break;
            }
            case QcomJobData::JT_QUIT:
                return;
            default:
                break;
            }

            m_tpc = this->Now();

            // TODO : wait for all substage finishing
        }
    }

    bool CommsQcom::IsPacketComplete(uint8_t buf[], int length)
    {

        if (length == 0)
            return false;

        if (length >= 255)
            return true;

        // error mark as complete
        if (buf[0] != QCOM_BROADCAST_SEEK_FC && buf[0] > m_egms.size())
        {
            return true;
        }

        return buf[1] == (length - 2);

    }

    bool CommsQcom::IsCRCValid(uint8_t buf[], int length)
    {
        return QSIM_CheckCRC(buf, static_cast<u16>(length)) != 0;// &&
               // ( m_resp_handler.find(((QCOM_RespMsgType*)buf)->DLL.FunctionCode) !=
               // m_resp_handler.end());
    }

    void CommsQcom::HandlePacket(uint8_t buf[], int length)
    {
        this->HandleResponse(buf, length);
    }

    void CommsQcom::HandleResponse(uint8_t buf[], int length)
    {
        QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

        //CORE_AUTO(it, m_resp_handler.find(p->DLL.FunctionCode));
        auto it = m_resp_handler.find(p->DLL.FunctionCode);
        if (it != m_resp_handler.end())
        {
            // we already check if handler exist in CRC check, so we can sure to get the handler
            // fix : delay check function code here to show error message
            CommsPacketHandlerPtr handler = it->second; //m_resp_handler[p->DLL.FunctionCode];

            if (handler->Parse(buf, length))
            {
                if (handler->HasSubStage())
                {
                    handler->AddJob();
                }

            }
        }
        else
        {
            COMMS_LOG(boost::format("Receive unrecognized response. Code:0x%|02X|\n") 
                % static_cast<uint32_t>(p->DLL.FunctionCode), CLL_Error);
        }
    }

    QcomDataPtr CommsQcom::GetEgmData(uint8_t poll_address)
    {
        if (poll_address >= 1)
        {
            std::unique_lock<std::mutex> lock(m_egms_guard);

            if (poll_address <= m_egms.size())
            {
                return m_egms[poll_address - 1];
            }
        }

        return nullptr;

    }

    QcomDataPtr CommsQcom::AddNewEgm()
    {
        // TODO : warning if larger than MAX_EGM_NUM

        QcomDataPtr p = MakeSharedPtr<QcomData>();
        std::memset(&p->data, 0, sizeof(QcomEGMData));

        // ref Qcom1.6-8.1.13 defaults value for EGM configuration after NVRAM clear
        p->data.custom.maxden = 0;
        p->data.custom.minrtp = 0;
        p->data.custom.maxrtp = 9999;
        p->data.custom.maxsd = 65535;
        p->data.custom.maxlines = 65535;
        p->data.custom.maxbet = 4294967295;
        p->data.custom.maxnpwin = 4294967295;
        p->data.custom.maxpwin = 4294967295;
        p->data.custom.maxect = 1000000;

        // ref Qcom1.6-8.1.17 defaults value for EGM Parameters Poll
        p->data.param.reserve = 1;
        p->data.param.lwin = 1000000;
        p->data.param.crlimit = 10000;
        p->data.param.dumax = 5;
        p->data.param.dulimit = 1000000;
        p->data.param.pwrtime = 900;
        p->data.param.eodt = 180;
        p->data.param.npwinp = 1000000;
        p->data.param.sapwinp = 1000000;

        std::unique_lock<std::mutex> lock(m_egms_guard);
        m_egms.push_back(p);
        return p;
    }

    size_t CommsQcom::GetEgmNum()
    {
        std::unique_lock<std::mutex> lock(m_egms_guard);

        return m_egms.size();
    }

//    QcomLocker CommsQcom::LockEGMData()
//    {
//        return QcomLocker(&m_egms, m_egms_guard);
//    }

    void CommsQcom::CaptureEGMData(std::vector<QcomDataPtr> &data)
    {
        std::unique_lock<std::mutex> lock(m_egms_guard);

        data.assign(m_egms.begin(), m_egms.end());
    }

    void CommsQcom::DoCheckCommsTimeout()
    {
        // Note: we should hanlde comms timeout, ref Qcom1.6-6.1.2
        // if we don't send poll to an EGM more than 10 secs then
        // it will go into comms timeout error and then we must send
        // poll address configuration again.
        // So I think we should check the time here and send a dummy poll
        // to prevent EGM changing status. But be careful when implementing this
        // it's possible that a user job is doing while the dummy poll try to send

        while(m_start)
        {
            // if seek egm is done, then
            // build poll general status
            // add the job as the top priority
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL); 
            QcomGenerlStatusPtr handler = std::static_pointer_cast<QcomGeneralStatus>(m_handler[QCOM_GSP_FC]);
            if (handler->BuildGeneralStatusPoll(job))
            {
                QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
                if (!m_lpbroadcast)
                {
                    if (broadcast_handler->BuildTimeDateBroadcast(job))
                    {
                        std::unique_lock<std::mutex> lock(m_job);
                        m_jobs.push_front(job);
                        m_job_cond.notify_one();
                    }
                }
                else
                {
                    QcomLinkedProgressiveData data;
                    if (this->GetLPConfigData(data))
                    {
                        if (broadcast_handler->BuildLinkProgressiveCurrentAmountBroadcast(job, data))
                        {
                            std::unique_lock<std::mutex> lock(m_job);
                            m_jobs.push_front(job);
                            m_job_cond.notify_one();
                        }
                    }
                }
            }

            // send every 8 secs in case job thread is too busy
            std::this_thread::sleep_for(std::chrono::seconds(8));
        }
    }

    void CommsQcom::PendingPoll(size_t poll_num)
    {
        if (!m_pending)
        {
            m_pending = true;
            m_pending_job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL, poll_num);
        }
        else
        {
            COMMS_LOG("A pending poll is working, duplicate pending will be ignored\n", CLL_Error);
        }
    }

    void CommsQcom::SendPoll()
    {
        if (m_pending)
        {
            this->AddJob(m_pending_job);
            m_pending_job = nullptr;
            m_pending = false;
        }
        else
        {
            COMMS_LOG("No available pending poll to send\n", CLL_Error);
        }
    }

    void CommsQcom::SeekEGM()
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST_SEEK);

        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastSeekPtr handler = std::static_pointer_cast<QcomBroadcastSeek>(m_handler[QCOM_BROADCAST_SEEK_FC]);
        if (!handler->BuildSeekEGMPoll(job))
            return;

        SG_QCOM_ADD_BROADCAST_JOB(job);
 
    }

    void CommsQcom::PollAddress(uint8_t poll_address)
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST);

        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_ADDRESS]);

        if (poll_address > 0)
        {
            if (!handler->BuildPollAddressPoll(job, poll_address))
                return;
        }
        else
        {
            if (!handler->BuildPollAddressPoll(job))
                return;
        }

        SG_QCOM_ADD_BROADCAST_JOB(job);
    }

    void CommsQcom::TimeData()
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST);

        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send Time Data Broadcast\n", CLL_Info);
        if (!handler->BuildTimeDateBroadcast(job))
            return;

        SG_QCOM_ADD_BROADCAST_JOB(job);
    }

    void CommsQcom::LinkJPCurrentAmount(QcomLinkedProgressiveData const & data)
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST);

        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send LP Current Amount Broadcast\n", CLL_Info);
        if (!handler->BuildLinkProgressiveCurrentAmountBroadcast(job, data))
            return;

        SG_QCOM_ADD_BROADCAST_JOB(job);
    }

    void CommsQcom::GeneralPromotional(std::string const & text)
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST);

        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send General Promotional Message Broadcast\n", CLL_Info);
        if (!handler->BuildGeneralPromotionalMessageBroadcast(job, text))
            return;

        SG_QCOM_ADD_BROADCAST_JOB(job);
    }

    void CommsQcom::SiteDetail(std::string const & stext, std::string const & ltext)
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_BROADCAST);

        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send Site Detail Broadcast\n", CLL_Info);
        if (!handler->BuildSiteDetailsBroadcast(job, stext, ltext))
            return;

        SG_QCOM_ADD_BROADCAST_JOB(job);
    }

    void CommsQcom::GeneralStatus(uint8_t poll_address)
    {
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomGenerlStatusPtr handler = std::static_pointer_cast<QcomGeneralStatus>(m_handler[QCOM_GSP_FC]);

        if (poll_address > 0)
        {
            if (!handler->BuildGeneralStatusPoll(job, poll_address))
                return;
        }
        else
        {
            if (!handler->BuildGeneralStatusPoll(job))
                return;
        }

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::EGMConfRequest(uint8_t poll_address, QcomEGMControlPollData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomEgmConfigurationRequestPtr handler = std::static_pointer_cast<QcomEgmConfigurationRequest>(m_handler[QCOM_EGMCRP_FC]);
        if (!handler->BuildEGMConfigReqPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::EGMConfiguration(uint8_t poll_address, QcomEGMConfigPollData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomEgmConfigurationPtr handler = std::static_pointer_cast<QcomEgmConfiguration>(m_handler[QCOM_EGMCP_FC]);
        if (!handler->BuildEGMConfigPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    bool CommsQcom::AddLPConfigData(uint8_t poll_address, uint16_t gvn, uint16_t pgid, uint8_t pnum, const QcomProgressiveConfigData &data)
    {
        bool is_lp = false;
        for (uint8_t level = 0; level < pnum; ++level)
        {
            if (data.flag_p[level])
            {
                is_lp = true;
                break;
            }
        }

        uint8_t err_pnum = QCOM_REMAX_EGMGCP;
        uint8_t err_level_type = QCOM_REMAX_EGMGCP;

        if (is_lp)
        {
            std::unique_lock<std::mutex> lock(m_lp_guard);

            QcomLinkProgressivePoolDataPtr ptr = MakeSharedPtr<QcomLinkProgressivePoolData>();
            ptr->config = data;
            ptr->pnum = pnum;
            ptr->configured = 0;
            std::memset(ptr->camt, 0, sizeof(ptr->camt));
            std::memset(ptr->overflow, 0, sizeof(ptr->overflow));

            auto res = m_lps.insert(std::make_pair(pgid, ptr));

            if (!res.second)
            {
                if (res.first->second->pnum == pnum)
                {
                    for (uint8_t level = 0; level < pnum; ++level)
                    {
                        if (res.first->second->config.flag_p[level] && data.flag_p[level])
                        {
                            //ptr->config.flag_p[level] = 1;
                            //ptr->config.sup[level] = data.sup[level];
                            //ptr->camt[level] = ptr->config.sup[level];
                        }
                        else if (!res.first->second->config.flag_p[level] && !data.flag_p[level])
                        {
                            //ptr->camt[level] = 0;
                        }
                        else
                        {
                            err_level_type = level;
                            break;
                        }
                    }

                    //if (err_level_type == QCOM_REMAX_EGMGCP)
                    //{
                    //    *(res.first->second) = *ptr;
                    //}
                }
                else
                {
                    err_pnum = res.first->second->pnum;
                }
            }
            else
            {
                for (uint8_t level = 0; level < ptr->pnum; ++level)
                {
                    ptr->camt[level] = ptr->config.init_contri[level];
                }
            }

            auto it = m_lp_egms.insert(std::make_pair(pgid, nullptr));
            if (it.second) // new pgid
            {
                LPEGMPtr pegms = MakeSharedPtr<LPEGM>();
                LPGameSetPtr pgames = MakeSharedPtr<LPGameSet>();
                pgames->insert(gvn);
                (*pegms)[poll_address] = pgames;
                it.first->second = pegms;
            }
            else
            {
                LPEGMPtr pegms = nullptr;
                if (!it.first->second) // should not happen
                {
                    pegms = MakeSharedPtr<LPEGM>();
                    it.first->second = pegms;
                }

                auto itegm = it.first->second->insert(std::make_pair(poll_address, nullptr));
                if (itegm.second) // new egm
                {
                    LPGameSetPtr pgames = MakeSharedPtr<LPGameSet>();
                    pgames->insert(gvn);
                    itegm.first->second = pgames;
                }
                else
                {
                    LPGameSetPtr pgames = nullptr;
                    if (!itegm.first->second) // should not happen
                    {
                        pgames = MakeSharedPtr<LPGameSet>();
                        itegm.first->second = pgames;
                    }

                    itegm.first->second->insert(gvn);
                }
            }
        }
        else
        {
            return false;
        }

        if (err_pnum != QCOM_REMAX_EGMGCP)
        {
            COMMS_LOG(boost::format("Configure same PGID 0x%|04X| with different configuration. Level num is %|| (%|| is expected).\n") %
                pgid % static_cast<uint32_t>(pnum) % static_cast<uint32_t>(err_pnum), CLL_Error);

            return false;
        }
        else if (err_level_type != QCOM_REMAX_EGMGCP)
        {
            COMMS_LOG(boost::format("Configure same PGID 0x%|04X| with different configuration. Level %|| is set to %||.\n") % 
                pgid % static_cast<uint32_t>(err_level_type) % (data.flag_p[err_level_type] == 1 ? std::string("SAP") : std::string("LP")), CLL_Error);
            
            return false;
        }

        return true;
    }

    bool CommsQcom::UpdateLPConfigData(uint16_t pgid, uint8_t pnum, QcomProgressiveConfigData const& data)
    {

        uint8_t err_pnum = QCOM_REMAX_EGMGCP;

        {
            std::unique_lock<std::mutex> lock(m_lp_guard);

            auto it = m_lps.find(pgid);

            if (it != m_lps.end())
            {
                if (it->second->pnum == pnum)
                {
                    for (uint8_t level = 0; level < pnum; ++level)
                    {
                        uint32_t turnover = 0;
                        if (it->second->configured)
                            turnover = it->second->camt[level] + it->second->overflow[level] - it->second->config.sup[level];

                        it->second->config.sup[level] = data.sup[level];
                        it->second->config.pinc[level] = data.pinc[level];
                        it->second->config.ceil[level] = data.ceil[level];
                        it->second->config.auxrtp[level] = data.auxrtp[level];

                        if (it->second->configured)
                            it->second->camt[level] = turnover + it->second->config.sup[level];
                        else if (it->second->camt[level] < it->second->config.sup[level])
                            it->second->camt[level] = it->second->config.sup[level];

                        if (it->second->camt[level] > it->second->config.ceil[level])
                        {
                            it->second->overflow[level] = it->second->camt[level] - it->second->config.ceil[level];
                            it->second->camt[level] = it->second->config.ceil[level];
                        }
                    }

                    it->second->configured = 1;
                }
                else
                {
                    err_pnum = it->second->pnum;
                }
            }
        }

        if (err_pnum != QCOM_REMAX_EGMGCP)
        {
            COMMS_LOG(
                boost::format(
                    "Can't update LP configuration. Progressive level num %|| mismatch game configuration setting num %||") % 
                static_cast<uint32_t>(pnum) % static_cast<uint32_t>(err_pnum), CLL_Error);

            return false;
        }

        return true;
    }

    bool CommsQcom::GetLPConfigData(QcomLinkedProgressiveData &data)
    {
        std::unique_lock<std::mutex> lock(m_lp_guard);

        if (m_curr_lp == m_lps.end())
            m_curr_lp = m_lps.begin();

        if (m_curr_lp != m_lps.end())
        {
            uint8_t num = 0;
            for (uint8_t i = 0; i < m_curr_lp->second->pnum; ++i)
            {
                if (m_curr_lp->second->config.flag_p[i])
                {
                    if (m_curr_lp->second->configured)
                    {
                        // uint32_t turnover = /*Get Turnover from EGM data, we don't do this, just fake one*/
                        // uint32_t current_contribution = turnover * m_curr_lp->second->pinc[i] / 10000;
                        // m_curr_lp->second->camt[i] = m_curr_lp->second->sup[i] + current_contribution;
                        m_curr_lp->second->camt[i] += 1; // fake
                        uint32_t amt = m_curr_lp->second->camt[i] + m_curr_lp->second->overflow[i];
                        if (amt > m_curr_lp->second->config.ceil[i])
                        {
                            m_curr_lp->second->overflow[i] = amt - m_curr_lp->second->config.ceil[i];
                            m_curr_lp->second->camt[i] = m_curr_lp->second->config.ceil[i];
                        }
                    }

                    data.lpamt[num] = m_curr_lp->second->camt[i];
                    data.plvl[num] = i;
                    data.pgid[num++] = m_curr_lp->first;
                }
            }

            SG_ASSERT(num);
            data.pnum = num;

            ++m_curr_lp;
        }
        else
        {
            return false;
        }

        return true;
    }

    bool CommsQcom::ReMapPGID(uint8_t poll_address, uint16_t gvn, uint16_t new_pgid, uint16_t old_pgid, uint8_t shared)
    {
        bool old_pgid_err = false;
        bool egm_no_pool_err = false;
        bool pool_no_egm_err = false;

        if (new_pgid != old_pgid)
        {
            std::unique_lock<std::mutex> lock(m_lp_guard);

            auto itoldpool = m_lps.find(old_pgid);
            auto itoldegms = m_lp_egms.find(old_pgid);
            auto itoldgameset = itoldegms->second->find(poll_address);
            auto itoldgame = itoldgameset->second->find(gvn);

            if (itoldpool != m_lps.end() && itoldegms != m_lp_egms.end() && itoldgameset != itoldegms->second->end() && itoldgame != itoldgameset->second->end())
            {
                auto itpool = m_lps.find(new_pgid);
                auto itegms = m_lp_egms.find(new_pgid);

                if (itpool == m_lps.end()) // new pgid
                {
                    if (itegms == m_lp_egms.end())
                    {
                        QcomLinkProgressivePoolDataPtr ptr = MakeSharedPtr<QcomLinkProgressivePoolData>();
                        m_lps[new_pgid] = ptr;

                        *ptr = *(itoldpool->second);

                        LPGameSetPtr pgames = nullptr;
                        if (!shared)
                        {
                            pgames = MakeSharedPtr<LPGameSet>();
                            pgames->insert(gvn);
                        }
                        else
                        {
                            pgames = itoldgameset->second;
                        }

                        LPEGMPtr pegms = MakeSharedPtr<LPEGM>();
                        (*pegms)[poll_address] = pgames;
                        m_lp_egms[new_pgid] = pegms;
                    }
                    else
                    {
                        egm_no_pool_err = true;
                    }
                }
                else
                {
                    if (itegms != m_lp_egms.end()) // exist pgid
                    {
                        if (!itegms->second)
                        {
                            itegms->second = MakeSharedPtr<LPEGM>();
                        }

                        auto itegm = itegms->second->insert(std::make_pair(poll_address, nullptr));
                        if (itegm.second)
                        {
                            if (!shared)
                            {
                                itegm.first->second = MakeSharedPtr<LPGameSet>();
                                itegm.first->second->insert(gvn);
                            }
                            else
                            {
                                itegm.first->second = itoldgameset->second;
                            }
                        }
                        else
                        {
                            if (!shared)
                            {
                                itegm.first->second->insert(gvn);
                            }
                            else
                            {
                                // actually I think this case should not happen
                                for (auto g : *(itoldgameset->second))
                                {
                                    itegm.first->second->insert(g);
                                }
                            }
                        }
                    }
                    else
                    {
                        pool_no_egm_err = true;
                    }
                }

                if (!pool_no_egm_err && !egm_no_pool_err)
                {
                    if (!shared)
                    {
                        itoldgameset->second->erase(itoldgame);
                        if (!itoldgameset->second->empty())
                        {
                            return true;
                        }
                    }

                    itoldgameset->second = nullptr;
                    itoldegms->second->erase(itoldgameset);
                    if (itoldegms->second->empty())
                    {
                        itoldegms->second = nullptr;
                        m_lp_egms.erase(itoldegms);
                    }

                    return true;
                }
            }
            else
            {
                old_pgid_err = true;
            }
        }
        else
        {
            return true; // no need to re-map
        }

        if (old_pgid_err)
        {
            COMMS_LOG(boost::format("No info of PGID 0x%|04X| found for EGM %|| Game(GVN 0x%|04X|) PGID change\n") %
                old_pgid % static_cast<uint32_t>(poll_address) % gvn, CLL_Error);
        }
        else if (egm_no_pool_err)
        {
            COMMS_LOG(boost::format("No Configuration info of PGID 0x%|04X| for EGM %|| Game(GVN 0x%|04X|) PGID change") %
                new_pgid % static_cast<uint32_t>(poll_address) % gvn, CLL_Error);
        }
        else if (pool_no_egm_err)
        {
            COMMS_LOG(boost::format("EGM data error of PGID 0x%|04X| for EGM %|| Game(GVN 0x5|04X|) PGID change") %
                new_pgid % static_cast<uint32_t>(poll_address) % gvn, CLL_Error);
        }

        return false;
    }

    bool CommsQcom::DecoratePoll(QcomPollPtr &p)
    {
        if (p)
        {
            QcomDataPtr pd = this->GetEgmData(p->poll.DLL.PollAddress);
            if (pd)
            {
                // reset last control here to refresh the latest status of last control
                // seems we've no need to lock data because at this time, no one can change last control?
                //std::unique_lock<std::mutex> lock(pd->locker);
                p->poll.DLL.ControlByte.CNTL = pd->data.control.last_control;
            }
            else
            {
                // this shouldn't happen
                SG_ASSERT(false);
                return false;
            }
        }

        // calculate CRC here to avoid duplicate computing
        PutCRC_LSBfirst(p->data, p->poll.DLL.Length);
        p->length = p->poll.DLL.Length + QCOM_CRC_SIZE;

        return true;
    }

    void CommsQcom::GameConfiguration(uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomGameConfigData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        if (m_pending)
        {
            COMMS_LOG("Can't use pending job when configure game\n", CLL_Error);
            return;
        }

        //if (!handler->BuildGameConfigPoll(job, poll_address, gvn, data))
        //    return;
        //std::vector<QcomJobDataPtr> jobs;
        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomGameConfigurationPtr handler = std::static_pointer_cast<QcomGameConfiguration>(m_handler[QCOM_EGMGCP_FC]);
        if (!handler->BuildGameConfigJobs(job, poll_address, gvn, pnum, data))
            return;

        if (this->AddLPConfigData(poll_address, gvn, data.settings.pgid, pnum, data.progressive))
        {
            m_lpbroadcast = true;
        }

        //for (size_t i = 0; i < jobs.size(); ++i)
        //{
        //   SG_QCOM_ADD_POLL_JOB(jobs[i]);
        //}

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::GameConfigurationChange(uint8_t poll_address, uint16_t gvn, QcomGameSettingData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        uint16_t old_pgid = 0;
        uint8_t shared = 0;

        QcomGameConfigurationChangePtr handler = std::static_pointer_cast<QcomGameConfigurationChange>(m_handler[QCOM_EGMVCP_FC]);

        if (!handler->BuildGameConfigChangePoll(job, poll_address, gvn, data, old_pgid, shared))
            return;

        if (old_pgid && old_pgid != data.pgid)
        {
            this->ReMapPGID(poll_address, gvn, data.pgid, old_pgid, shared);
        }

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::PurgeEvents(uint8_t poll_address, uint8_t evtno)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomPurgeEventsPtr handler = std::static_pointer_cast<QcomPurgeEvents>(m_handler[QCOM_PEP_FC]);
        if (!handler->BuildPurgeEventsPoll(job, poll_address, evtno))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::EGMParameters(uint8_t poll_address, QcomEGMParametersData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomEgmParametersPtr handler = std::static_pointer_cast<QcomEgmParameters>(m_handler[QCOM_EGMPP_FC]);
        if (!handler->BuildEgmParametersPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::ProgressiveChange(uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        uint16_t pgid = 0;

        QcomProgressiveConfigPtr handler = std::static_pointer_cast<QcomProgressiveConfig>(m_handler[QCOM_PCP_FC]);
        if (!handler->BuildProgConfigPoll(job, poll_address, gvn, pnum, data, pgid))
            return;

        if (pgid != 0 && pgid != 0xFFFF) // a valid lp pgid
        {
            this->UpdateLPConfigData(pgid, pnum, data);
        }

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::ExtJPInfo(uint8_t poll_address, QcomExtJPInfoData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomExtJPInfoPtr handler = std::static_pointer_cast<QcomExtJPInfo>(m_handler[QCOM_EXTJIP_FC]);
        if (!handler->BuildExtJPInfoPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::ProgHashRequest(uint8_t poll_address, QcomProgHashRequestData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomProgHashRequestPtr handler = std::static_pointer_cast<QcomProgHashRequest>(m_handler[QCOM_PHRP_FC]);
        if (!handler->BuildProgHashRequstPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::SystemLockup(uint8_t poll_address, QcomSysLockupRequestData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomSysLockupReqPtr handler = std::static_pointer_cast<QcomSysLockupReq>(m_handler[QCOM_SALRP_FC]);
        if (!handler->BuildSysLockupReqPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::CashTicketOutAck(uint8_t poll_address, QcomCashTicketOutRequestAckPollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomCashTicketOutRequestAckPtr handler = std::static_pointer_cast<QcomCashTicketOutRequestAck>(m_handler[QCOM_TORACKP_FC]);
        if (!handler->BuildCashTicketOutRequestAckPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::CashTicketInAck(uint8_t poll_address, QcomCashTicketInRequestAckPollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomCashTicketInRequestAckPtr handler = std::static_pointer_cast<QcomCashTicketInRequestAck>(m_handler[QCOM_TIRACKP_FC]);
        if (!handler->BuildCashTicketInRequestAckPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::CashTicketOutReqeust(uint8_t poll_address)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomCashTicketOutRequestPtr handler = std::static_pointer_cast<QcomCashTicketOutRequest>(m_handler[QCOM_CCLRP_FC]);
        if (!handler->BuildCashTicketOutRequestPoll(job, poll_address))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::EGMGeneralMaintenance(uint8_t poll_address, uint16_t gvn, QcomEGMGeneralMaintenancePollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomEgmGeneralMaintenancePtr handler = std::static_pointer_cast<QcomEgmGeneralMaintenance>(m_handler[QCOM_EGMGMP_FC]);
        if (!handler->BuildEgmGeneralMaintenancePoll(job, poll_address, gvn, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::RequestAllLoggedEvents(uint8_t poll_address)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomRequestAllLoggedEventsPtr handler = std::static_pointer_cast<QcomRequestAllLoggedEvents>(m_handler[QCOM_RALEP_FC]);
        if (!handler->BuildRequestAllLoggedEventsPoll(job, poll_address))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::NoteAcceptorMaintenance(uint8_t poll_address, QcomNoteAcceptorMaintenanceData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomNoteAcceptorMaintenancePtr handler = std::static_pointer_cast<QcomNoteAcceptorMaintenance>(m_handler[QCOM_NAMP_FC]);
        if (!handler->BuildNoteAcceptorMaintenancePoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::HopperTicketPrinterMaintenance(uint8_t poll_address, uint8_t test, QcomHopperTicketPrinterData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomHTPMaintenancePtr handler = std::static_pointer_cast<QcomHTPMaintenance>(m_handler[QCOM_HTPMP_FC]);
        if (!handler->BuildHTPMaintenancePoll(job, poll_address, test, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::LPAwardAck(uint8_t poll_address)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomLPAwardAckPtr handler = std::static_pointer_cast<QcomLPAwardAck>(m_handler[QCOM_LPAAP_FC]);
        if (!handler->BuildLPAwardAckPoll(job, poll_address))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::GeneralReset(uint8_t poll_address, QcomGeneralResetPollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomGeneralResetPtr handler = std::static_pointer_cast<QcomGeneralReset>(m_handler[QCOM_EGMGRP_FC]);
        if (!handler->BuildGeneralResetPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::SPAM(uint8_t poll_address, uint8_t type, QcomSPAMPollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        if (type == QCOM_SPAM_TYPE_A)
        {
            QcomSPAMAPtr handler = std::static_pointer_cast<QcomSPAMA>(m_handler[QCOM_SPAMA_FC]);
            if (!handler->BuildSPAMAPoll(job, poll_address, data))
                return;
        }
        else
        {
            QcomSPAMBPtr handler = std::static_pointer_cast<QcomSPAMB>(m_handler[QCOM_SPAMB_FC]);
            if (!handler->BuildSPAMBPoll(job, poll_address, data))
                return;
        }

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::TowerLightMaintenance(uint8_t poll_address, QcomTowerLightMaintenancePollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomTowerLightMaintenancePtr handler = std::static_pointer_cast<QcomTowerLightMaintenance>(m_handler[QCOM_EGMTLMP_FC]);
        if (!handler->BuildTowerLightMaintenancePoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::ECTToEGM(uint8_t poll_address, QcomECTToEGMPollData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomECTToEGMPtr handler = std::static_pointer_cast<QcomECTToEGM>(m_handler[QCOM_ECTEGMP_FC]);
        if (!handler->BuildECTToEGMPoll(job, poll_address, data))
            return;

        SG_QCOM_ADD_POLL_JOB(job);

    }

    void CommsQcom::ECTFromEGMRequest(uint8_t poll_address)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomECTFromEGMRequestPtr handler = std::static_pointer_cast<QcomECTFromEGMRequest>(m_handler[QCOM_ECTEGMLRP_FC]);
        if (!handler->BuildECTFromEGMRequestPoll(job, poll_address))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::ECTLockupReset(uint8_t poll_address, uint8_t denied)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomECTLockupResetPtr handler = std::static_pointer_cast<QcomECTLockupReset>(m_handler[QCOM_ECTLRP_FC]);
        if (!handler->BuildECTLockupResetPoll(job, poll_address, denied))
            return;

        SG_QCOM_ADD_POLL_JOB(job);
    }
}



