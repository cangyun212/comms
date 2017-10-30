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

        // TODO : ...

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
        return QSIM_CheckCRC(buf, static_cast<u16>(length)) &&
                ( m_resp_handler.find(((QCOM_RespMsgType*)buf)->DLL.FunctionCode) !=
                m_resp_handler.end());
    }

    void CommsQcom::HandlePacket(uint8_t buf[], int length)
    {
        this->HandleResponse(buf, length);
    }

    void CommsQcom::HandleResponse(uint8_t buf[], int length)
    {
        QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;

        //CORE_AUTO(it, m_resp_handler.find(p->DLL.FunctionCode));
        //if (it != m_resp_handler.end())
        //{
            // we already check if handler exist in CRC check, so we can sure to get the handler
            CommsPacketHandlerPtr handler = m_resp_handler[p->DLL.FunctionCode];

            if (handler->Parse(buf, length))
            {
                if (handler->HasSubStage())
                {
                    handler->AddJob();
                }

            }
        //}
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

    bool CommsQcom::AddLPConfigData(uint16_t pgid, const QcomProgressiveConfigData &data)
    {
        bool is_lp = false;
        for (uint8_t level = 0; level < data.pnum; ++level)
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
            auto res = m_lps.insert(std::make_pair(pgid, MakeSharedPtr<QcomProgressiveConfigData>(data)));

            if (!res.second)
            {
                QcomProgressiveConfigData tmp;
                if (res.first->second->pnum == data.pnum)
                {
                    tmp.pnum = data.pnum;
                    for (uint8_t level = 0; level < data.pnum; ++level)
                    {
                        if (res.first->second->flag_p[level] && data.flag_p[level])
                        {
                            tmp.flag_p[level] = 1;
                            tmp.camt[level] = data.camt[level];
                        }
                        else if (!res.first->second->flag_p[level] && !data.flag_p[level])
                        {
                            tmp.flag_p[level] = 0;
                            tmp.camt[level] = 0;
                        }
                        else
                        {
                            err_level_type = level;
                            break;
                        }
                    }

                    if (err_level_type == QCOM_REMAX_EGMGCP)
                    {
                        for (uint8_t level = 0; level < tmp.pnum; ++level)
                        {
                            if (tmp.flag_p[level])
                            {
                                res.first->second->camt[level] = tmp.camt[level];
                            }
                        }
                    }
                }
                else
                {
                    err_pnum = res.first->second->pnum;
                }
            }
        }

        if (err_pnum != QCOM_REMAX_EGMGCP)
        {
            COMMS_LOG(boost::format("Can't update link progressive data of PGID %1% due to wrong level num %2% (%3% is expected).\n") %
                pgid % static_cast<uint32_t>(data.pnum) % static_cast<uint32_t>(err_pnum), CLL_Error);

            return false;
        }
        else if (err_level_type != QCOM_REMAX_EGMGCP)
        {
            COMMS_LOG(boost::format("Can't update link progressive data of PGID %1% due to wrong level type. Level %2% should be %3%.\n") % 
                pgid % static_cast<uint32_t>(err_level_type) % (data.flag_p[err_level_type] == 1 ? std::string("SAP") : std::string("LP")), CLL_Error);
            
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
                if (m_curr_lp->second->flag_p[i])
                {
                    m_curr_lp->second->camt[i] += 1; // TODO: increment lp here implicitly
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

    void CommsQcom::GameConfiguration(uint8_t poll_address, uint16_t gvn, QcomGameConfigData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        //SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);
        if (m_pending)
        {
            COMMS_LOG("Can't use pending job when configure game\n", CLL_Error);
            return;
        }

        QcomGameConfigurationPtr handler = std::static_pointer_cast<QcomGameConfiguration>(m_handler[QCOM_EGMGCP_FC]);
        //if (!handler->BuildGameConfigPoll(job, poll_address, gvn, data))
        //    return;
        std::vector<QcomJobDataPtr> jobs;
        if (!handler->BuildGameConfigJobs(jobs, poll_address, gvn, data))
            return;

        for (uint8_t i = 0; i < data.progressive_config.pnum; ++i)
        {
            if (data.progressive_config.flag_p[i])
            {
                // from now, sim should keep sending link broadcast instead date and time only
                m_lpbroadcast = true;
                break;
            }
        }

        for (size_t i = 0; i < jobs.size(); ++i)
        {
            SG_QCOM_ADD_POLL_JOB(jobs[i]);
        }

        //SG_QCOM_ADD_POLL_JOB(job);
    }

    void CommsQcom::GameConfigurationChange(uint8_t poll_address, uint16_t gvn, QcomGameSettingData const& data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomGameConfigurationChangePtr handler = std::static_pointer_cast<QcomGameConfigurationChange>(m_handler[QCOM_EGMVCP_FC]);

        if (!handler->BuildGameConfigChangePoll(job, poll_address, gvn, data))
            return;

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

    void CommsQcom::ProgressiveChange(uint8_t poll_address, uint16_t gvn, QcomProgressiveChangeData const & data)
    {
        SG_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());

        SG_QCOM_MAKE_JOB(job, QcomJobData::JT_POLL);

        QcomProgressiveConfigPtr handler = std::static_pointer_cast<QcomProgressiveConfig>(m_handler[QCOM_PCP_FC]);
        if (!handler->BuildProgConfigPoll(job, poll_address, gvn, data))
            return;

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
}



