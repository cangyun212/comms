#include "core/core.hpp"
#include "core/core_utils.hpp"
#include "core/console/core_console_printer.hpp"

#include <iostream>

#include "comms/qcom/qcom.hpp"
#include "comms/qcom/qogr/qogr_crc.h"
#include "comms/qcom/qcom_broadcast_seek.hpp"
#include "comms/qcom/qcom_broadcast.hpp"
#include "comms/qcom/qcom_egm_config_req.hpp"
#include "comms/qcom/qcom_egm_config.hpp"
#include "comms/qcom/qcom_general_status.hpp"
#include "comms/qcom/qcom_game_config.hpp"

// Typically 32, max 250
#define MAX_EGM_NUM 32

namespace sg {

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
        Quit();
    }

    void CommsQcom::DoInit()
    {
        CommsPtr base= shared_from_this();
        CommsQcomPtr this_ptr = static_pointer_cast<CommsQcom>(base);

        CommsPacketHandlerPtr p = MakeSharedPtr<QcomBroadcastSeek>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(QCOM_SEEKEGM_FC, p));

        p = MakeSharedPtr<QcomEgmConfigurationRequest>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));
        m_resp_handler.insert(std::make_pair(QCOM_EGMCR_FC, p));

        p = MakeSharedPtr<QcomEgmConfiguration>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomGeneralStatus>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomBroadcast>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        p = MakeSharedPtr<QcomGameConfiguration>(this_ptr);
        m_handler.insert(std::make_pair(p->Id(), p));

        // TODO : ...

    }

    CommsPacketHandlerPtr CommsQcom::GetHandler(uint8_t id) const
    {
        CORE_AUTO(it, m_handler.find(id));
        if (it != m_handler.end())
        {
            return it->second;
        }

        return nullptr;
    }

    void CommsQcom::DoStart()
    {
        this->StartJobThread();

        CORE_FOREACH(HandlerType::reference handler, m_handler)
        {
            handler.second->StartSubStageThread();
        }

    }

    void CommsQcom::DoStop()
    {
        QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_QUIT);
        AddJob(job);

        this->StopJobThread();

        CORE_FOREACH(HandlerType::reference handler, m_handler)
        {
            handler.second->StopSubStageThread();
        }

    }

    void CommsQcom::StartJobThread()
    {
        m_worker = thread(bind(&CommsQcom::RunJob, this));
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
        unique_lock<mutex> lock(m_job);
        m_jobs.push_back(job_data);
        m_job_cond.notify_one();
    }

    void CommsQcom::RunJob()
    {
        while (1) {

            unique_lock<mutex> lock(m_job);
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
        if (buf[0] != QCOM_BROADCAST_SEEK_FC && buf[0] >= m_egms.size())
        {
            return true;
        }

        return buf[1] == (length - 2);

    }

    bool CommsQcom::IsCRCValid(uint8_t buf[], int length)
    {
        return QSIM_CheckCRC(buf, length) &&
                ( m_resp_handler.find(((QCOM_RespMsgType*)buf)->DLL.FunctionCode) !=
                m_resp_handler.end());
    }

    void CommsQcom::HandlePacket(uint8_t buf[], int length)
    {
        HandleResponse(buf, length);
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
            shared_lock<shared_mutex> lock(m_egms_guard);

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

        unique_lock<shared_mutex> lock(m_egms_guard);

        QcomDataPtr p = MakeSharedPtr<QcomData>();
        std::memset(&p->data, 0, sizeof(EGMData));
        m_egms.push_back(p);
        return p;
    }

    void CommsQcom::GetEgmData(std::vector<QcomDataPtr> &data)
    {
        shared_lock<shared_mutex> lock(m_egms_guard);

        data.assign(m_egms.begin(), m_egms.end());
    }

    size_t CommsQcom::GetEgmNum()
    {
        shared_lock<shared_mutex> lock(m_egms_guard);

        return m_egms.size();
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
            QcomGenerlStatusPtr handler = static_pointer_cast<QcomGeneralStatus>(m_handler[QCOM_GSP_FC]);
            QcomJobDataPtr job = handler->MakeGeneralStatusJob();
            if (job)
            {
                unique_lock<mutex> lock(m_job);
                m_jobs.push_front(job);
                m_job_cond.notify_one();
            }

            QcomBroadcastPtr broadcast_handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
            broadcast_handler->BuildTimeDataBroadcast();

            // send every 8 secs in case job thread is too busy
            this_thread::sleep_for(chrono::seconds(8));
        }
    }

    // TODO:
    void CommsQcom::SeekEGM()
    {
        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastSeekPtr handler = static_pointer_cast<QcomBroadcastSeek>(m_handler[QCOM_BROADCAST_SEEK_FC]);
        handler->BuildSeekEGMPoll();
        QcomBroadcastPtr broadcast_handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::PollAddress(uint8_t poll_address)
    {
        // we have no need to protect m_handler since no one will touch it after init
        QcomBroadcastPtr handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_ADDRESS]);

        if (poll_address > 0)
        {
            handler->BuildPollAddressPoll(poll_address);
        }
        else
        {
            handler->BuildPollAddressPoll();
        }
    }

    void CommsQcom::EGMConfRequest(uint8_t poll_address, uint8_t mef, uint8_t gcr, uint8_t psn)
    {
        QcomEgmConfigurationRequestPtr handler = static_pointer_cast<QcomEgmConfigurationRequest>(m_handler[QCOM_EGMCRP_FC]);
        handler->BuildEGMConfigReqPoll(poll_address, mef, gcr, psn);
        QcomBroadcastPtr broadcast_handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }

    void CommsQcom::EGMConfiguration(uint8_t poll_address, uint8_t jur, uint32_t den, uint32_t tok, uint32_t maxden, uint16_t minrtp, uint16_t maxrtp, uint16_t maxsd,
                            uint16_t maxlines, uint32_t maxbet, uint32_t maxnpwin, uint32_t maxpwin, uint32_t maxect)
    {
        QcomEgmConfigurationPtr handler = static_pointer_cast<QcomEgmConfiguration>(m_handler[QCOM_EGMCP_FC]);
        handler->BuildEGMConfigPoll(poll_address, jur, den, tok, maxden, minrtp, maxrtp, maxsd, maxlines, maxbet, maxnpwin, maxpwin, maxect);
        QcomBroadcastPtr broadcast_handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }
    void CommsQcom::SendBroadcast(uint32_t broadcast_type, std::string gpm_text, std::string sds_text, std::string sdl_text)
    {
        QcomBroadcastPtr handle = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);

        switch (broadcast_type) {
        case BROADCAST_TYPE_SEEK_EGM:
        {
            QcomBroadcastSeekPtr handle = static_pointer_cast<QcomBroadcastSeek>(m_handler[QCOM_BROADCAST_SEEK_FC]);
            std::cout << "Send Seek Egm Broadcast" <<std::endl;
            handle->BuildSeekEGMPoll();
        }
            break;
        case BROADCAST_TYPE_TIME_DATA:
        {
            std::cout << "Send Time Data Broadcast" <<std::endl;
            handle->BuildTimeDataBroadcast();
        }
            break;
        case BROADCAST_TYPE_POLL_ADDRESS:
        {
            std::cout << "Send Poll Address Broadcast" <<std::endl;
            handle->BuildPollAddressPoll();
        }
            break;
        case BROADCAST_TYPE_LINK_JP_CUR_AMOUNT:
        {
            std::cout << "Send LJP Current Amount Broadcast" <<std::endl;
            handle->BuildLinkProgressiveCurrentAmountBroadcast();
        }
            break;
        case BROADCAST_TYPE_GPM:
            if(gpm_text.length() <= QCOM_BMGPM_TEXT_SIZE)
            {
                std::cout << "gpm text length = " << gpm_text.length() <<std::endl;
                handle->BuildGeneralPromotionalMessageBroadcast(gpm_text.length(), gpm_text.c_str());
            }
            break;
        case BROADCAST_TYPE_SITE_DETAILS:
            if((sds_text.length() > 0 || sdl_text.length() > 0) && sds_text.length() <= QCOM_BMSD_SLEN && sdl_text.length() <= QCOM_BMSD_LLEN)
            {
                std::cout << "sd text = " << sds_text;
                std::cout << " + " << sdl_text << std::endl;
                handle->BuildSiteDetailsBroadcast(sds_text.length(), sdl_text.length(), sds_text.c_str(), sdl_text.c_str());
            }
            break;
        default:
            std::cout <<"qcom default" <<std::endl;
            break;
        }
    }

    void CommsQcom::GameConfiguration(uint8_t poll_address, uint8_t var, uint8_t varlock, uint8_t gameenable, uint8_t pnum, const std::vector<uint8_t> &lp, const std::vector<uint32_t> &amct)
    {
        QcomGameConfigurationPtr handler = static_pointer_cast<QcomGameConfiguration>(m_handler[QCOM_EGMGCP_FC]);
        handler->BuildGameConfigPoll(poll_address, var, varlock, gameenable, pnum, lp, amct);
        QcomBroadcastPtr broadcast_handler = static_pointer_cast<QcomBroadcast>(m_handler[QCOM_BROADCAST_POLL_FC]);
        broadcast_handler->BuildTimeDataBroadcast();
    }
}



