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

// Typically 32, max 250
#define MAX_EGM_NUM 32

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

    QcomPollPtr QcomJobData::GetPoll(size_t poll_address) const
    {
        return m_polls[poll_address];
    }

    void QcomJobData::AddPoll(const QcomPollPtr &poll)
    {
        m_polls.push_back(poll);
    }

    size_t QcomJobData::GetBroadcastNum() const
    {
        return m_broadcast.size();
    }

    QcomPollPtr QcomJobData::GetBroadcast(size_t index) const
    {
        return m_broadcast[index];
    }

    void QcomJobData::AddBroadcast(const QcomPollPtr &broadcast)
    {
        m_broadcast.push_back(broadcast);
    }

    CommsQcom::CommsQcom(const std::string &dev)
        : Comms(dev, Comms::CT_QCOM)
    {
        m_egms.reserve(MAX_EGM_NUM);
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

            switch (job->GetType())
            {
            case QcomJobData::JT_POLL:
            {
                size_t num = job->GetPollNum();
                for (size_t i = 0; i < num; ++i)
                {
                    // TODO : check and reset tcp time
                    QcomPollPtr poll = job->GetPoll(i);
                    //unique_lock<mutex> rsp_lock(m_response);
                    this->SendPacket(poll->data, poll->length);
                    //m_resp_timeout = false;
                    //m_response_cond.wait_for(rsp_lock, chrono::milliseconds(5)); // Ref Qcom1.6-14.1.7
                    //if (!m_resp_received)
                    //{
                        // TODO : log warning for this
                    //    m_resp_timeout = true;
                    //}
                }

                num = job->GetBroadcastNum(); // at least 1 broadcast exist in 1 poll cycle
                for (size_t i = 0; i < num; ++i)
                {
                    QcomPollPtr poll = job->GetBroadcast(i);
                    this->SendPacket(poll->data, poll->length);
                }

                break;
            }
            case QcomJobData::JT_BROADCAST:
            case QcomJobData::JT_BROADCAST_SEEK:
            {
                size_t num = job->GetBroadcastNum();
                for (size_t i = 0; i < num; ++i)
                {
                    QcomPollPtr poll = job->GetBroadcast(i);
                    this->SendPacket(poll->data, poll->length);
                }
                break;
            }
            case QcomJobData::JT_QUIT:
                return;
            default:
                break;
            }

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
        // TODO: we should hanlde comms timeout, ref Qcom1.6-6.1.2
        // if we don't send poll to an EGM more than 10 secs then
        // it will go into comms timeout error and then we must send
        // poll address configuration again.
        // So I think we should check the time here and send a dummy poll
        // to prevent EGM changing status. But be carefule when implementing this
        // it's possible that a user job is doing while the dummy poll try to send

        while(m_start)
        {
            // if seek egm is done, then
            // build poll general status
            // add the job as the top priority
            QcomGenerlStatusPtr handler = std::static_pointer_cast<QcomGeneralStatus>(m_handler[QCOM_GSP_FC]);
            QcomJobDataPtr job = handler->MakeGeneralStatusJob();
            if (job)
            {
                std::unique_lock<std::mutex> lock(m_job);
                m_jobs.push_front(job);
                m_job_cond.notify_one();
            }

            QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
            broadcast_handler->BuildTimeDataBroadcast();

            // send every 8 secs in case job thread is too busy
            std::this_thread::sleep_for(std::chrono::seconds(8));
        }
    }

    // TODO:
    void CommsQcom::SeekEGM()
    {
        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastSeekPtr handler = std::static_pointer_cast<QcomBroadcastSeek>(m_handler[QCOM_BROADCAST_SEEK_FC]);
        handler->BuildSeekEGMPoll();
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::PollAddress(uint8_t poll_address)
    {
        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastPtr handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_ADDRESS]);

        if (poll_address > 0)
        {
            handler->BuildPollAddressPoll(poll_address);
        }
        else
        {
            handler->BuildPollAddressPoll();
        }
    }

    void CommsQcom::TimeData()
    {
        QcomBroadcastPtr handle = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send Time Data Broadcast\n", CLL_Info);
        handle->BuildTimeDataBroadcast();
    }

    void CommsQcom::LinkJPCurrentAmount(QcomLinkedProgressiveData const & data)
    {
        QcomBroadcastPtr handle = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send LP Current Amount Broadcast\n", CLL_Info);
        handle->BuildLinkProgressiveCurrentAmountBroadcast(data);
    }

    void CommsQcom::GeneralPromotional(std::string const & text)
    {
        QcomBroadcastPtr handle = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send General Promotional Message Broadcast\n", CLL_Info);
        handle->BuildGeneralPromotionalMessageBroadcast(text);
    }

    void CommsQcom::SiteDetail(std::string const & stext, std::string const & ltext)
    {
        QcomBroadcastPtr handle = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        COMMS_LOG("Send Site Detail Broadcast\n", CLL_Info);
        handle->BuildSiteDetailsBroadcast(stext, ltext);
    }

    void CommsQcom::EGMConfRequest(uint8_t poll_address, QcomEGMControlPollData const & data)
    {
        BOOST_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());
        QcomEgmConfigurationRequestPtr handler = std::static_pointer_cast<QcomEgmConfigurationRequest>(m_handler[QCOM_EGMCRP_FC]);
        handler->BuildEGMConfigReqPoll(poll_address, data);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::EGMConfiguration(uint8_t poll_address, QcomEGMConfigPollData const & data)
    {
        BOOST_ASSERT(poll_address > 0 && poll_address <= this->GetEgmNum());
        QcomEgmConfigurationPtr handler = std::static_pointer_cast<QcomEgmConfiguration>(m_handler[QCOM_EGMCP_FC]);
        handler->BuildEGMConfigPoll(poll_address, data);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::GameConfiguration(uint8_t poll_address, uint16_t gvn, QcomGameConfigPollData const& data)
    {
        QcomGameConfigurationPtr handler = std::static_pointer_cast<QcomGameConfiguration>(m_handler[QCOM_EGMGCP_FC]);
        handler->BuildGameConfigPoll(poll_address, gvn, data);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::GameConfigurationChange(uint8_t poll_address, uint16_t gvn, QcomGameSettingData const& data)
    {
        QcomGameConfigurationChangePtr handler = std::static_pointer_cast<QcomGameConfigurationChange>(m_handler[QCOM_EGMVCP_FC]);
        handler->BuildGameConfigChangePoll(poll_address, gvn, data);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::PurgeEvents(uint8_t poll_address, uint8_t evtno)
    {
        QcomPurgeEventsPtr handler = std::static_pointer_cast<QcomPurgeEvents>(m_handler[QCOM_PEP_FC]);
        handler->BuildPurgeEventsPoll(poll_address, evtno);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::EGMParameters(uint8_t poll_address, QcomEGMParametersData const& data)
    {
        QcomEgmParametersPtr handler = std::static_pointer_cast<QcomEgmParameters>(m_handler[QCOM_EGMPP_FC]);
        handler->BuildEgmParametersPoll(poll_address, data);
        QcomBroadcastPtr broadcast_handler = std::static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }
}



