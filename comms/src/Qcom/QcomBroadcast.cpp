 
#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{

    uint8_t QcomBroadcast::Id() const
    {
        return QCOM_BROADCAST_ADDRESS;
    }

    QcomPollPtr QcomBroadcast::MakePollAddressPoll(size_t size)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;

        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        poll->poll.Data.Broadcast.ESIZ = sizeof(qc_egmpactype);
        poll->poll.Data.Broadcast.extd.EXTD.egmpac.EFUNC = QCOM_BMEGMPAC_FC;
        poll->poll.Data.Broadcast.extd.EXTD.egmpac.ESIZ = static_cast<u8>(size * sizeof(qc_egmpacretype));
        poll->poll.Data.Broadcast.extd.EXTD.egmpac.NUM = static_cast<u8>(size);

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    QcomJobDataPtr QcomBroadcast::MakePollAddressJob()
    {
        if (auto it = m_qcom.lock())
        {
            std::vector<QcomDataPtr> egmDatas;
            it->GetEgmData(egmDatas);
            size_t size = egmDatas.size();

            if(!size)
            {
                return nullptr;
            }

            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

            // batch by QCOM_REMAX_BMEGMPAC, ref Qcom1.6-15.5.4
            size_t group = size / QCOM_REMAX_BMEGMPAC;
            size_t extra = size % QCOM_REMAX_BMEGMPAC;

            QcomPollPtr poll_extra = this->MakePollAddressPoll(extra);
            for (size_t i = 0; i < extra; ++i)
            {
                std::unique_lock<std::mutex> lock(egmDatas[i]->locker);
                poll_extra->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].SN.SER =egmDatas[i]->data.serialMidBCD;
                poll_extra->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].PADR = static_cast<u8>(i + 1);
                egmDatas[i]->data.poll_address = static_cast<u8>(i + 1);
            }
            PutCRC_LSBfirst(poll_extra->data, poll_extra->poll.DLL.Length);
            job->AddBroadcast(poll_extra);

            for (size_t j = 0; j < group; ++j)
            {
                QcomPollPtr poll = this->MakePollAddressPoll(QCOM_REMAX_BMEGMPAC);
                size_t low_bound = extra + j * QCOM_REMAX_BMEGMPAC;
                size_t high_bound = low_bound + QCOM_REMAX_BMEGMPAC;
                for (size_t i = low_bound; i < high_bound; ++i)
                {
                    std::unique_lock<std::mutex> lock(egmDatas[i]->locker);
                    poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].SN.SER =egmDatas[i]->data.serialMidBCD;
                    poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].PADR = static_cast<u8>(i + 1);
                    egmDatas[i]->data.poll_address = static_cast<u8>(i + 1);
                }
                PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
                job->AddBroadcast(poll);
            }

            return job;
        }

        return nullptr;
    }

    void QcomBroadcast::BuildPollAddressPoll()
    {
        if (auto it = m_qcom.lock())
        {
            QcomJobDataPtr job = this->MakePollAddressJob();
            if (job)
            {
                it->AddJob(job);
            }
        }
    }

    void QcomBroadcast::BuildPollAddressPoll(uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr d = it->GetEgmData(poll_address);

            if (d)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

                QcomPollPtr poll = this->MakePollAddressPoll(1);

                std::unique_lock<std::mutex> lock(d->locker);
                poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[0].SN.SER = d->data.serialMidBCD;
                poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[0].PADR = poll_address;
                d->data.poll_address = poll_address;
                lock.unlock();

                PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

                job->AddBroadcast(poll);

                it->AddJob(job);
            }
        }
    }

    //Time Data broadcast
    QcomPollPtr QcomBroadcast::MakeTimeDataBroadcast()
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;
        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        poll->poll.Data.Broadcast.ESIZ = 0;

        time_t now_time = time(NULL);
        struct tm cTime;

        SecondsToCalendar(now_time, &cTime);
        _QComPutBCD(cTime.tm_year, &poll->poll.Data.Broadcast.TIMEDATE.year, sizeof(poll->poll.Data.Broadcast.TIMEDATE.year));
        _QComPutBCD(cTime.tm_mon + 1, &poll->poll.Data.Broadcast.TIMEDATE.month,
                    sizeof(poll->poll.Data.Broadcast.TIMEDATE.month));
        _QComPutBCD(cTime.tm_mday, &poll->poll.Data.Broadcast.TIMEDATE.day, sizeof(poll->poll.Data.Broadcast.TIMEDATE.day));
        _QComPutBCD(cTime.tm_hour, &poll->poll.Data.Broadcast.TIMEDATE.hours, sizeof(poll->poll.Data.Broadcast.TIMEDATE.hours));
        _QComPutBCD(cTime.tm_min, &poll->poll.Data.Broadcast.TIMEDATE.minutes,
                    sizeof(poll->poll.Data.Broadcast.TIMEDATE.minutes));
        _QComPutBCD(cTime.tm_sec, &poll->poll.Data.Broadcast.TIMEDATE.seconds,
                    sizeof(poll->poll.Data.Broadcast.TIMEDATE.seconds));
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomBroadcast::BuildTimeDataBroadcast()
    {
        if (auto it = m_qcom.lock())
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

            QcomPollPtr poll = this->MakeTimeDataBroadcast();

            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

            job->AddBroadcast(poll);

            it->AddJob(job);
        }
    }

    //Link Progressive Current Amount broadcast
    QcomPollPtr QcomBroadcast::MakeLinkProgressiveCurrentAmountBroadcast()
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;

        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        //TEMP Link Jackpot Progressive Data for default
        WORD tmp_pgid = 0x0001;
        unsigned int tmp_jackpot_levels = 3;
        unsigned int tmp_jackpot_amounts[3] = {100001, 10002, 1003};
        poll->poll.Data.Broadcast.ESIZ = static_cast<u8>(sizeof(qc_lpcatype) + sizeof(qc_lpcaretype) * (tmp_jackpot_levels));
        poll->poll.Data.Broadcast.extd.EXTD.lpca.EFUNC = QCOM_BMLPCA_FC;
        poll->poll.Data.Broadcast.extd.EXTD.lpca.NUM.bits.levels = tmp_jackpot_levels - 1;
        size_t i = 0;
        for(i = 0; i < tmp_jackpot_levels; ++i)
        {
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].PGID = tmp_pgid;
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].PLVL.bits.level = i;
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].CAMT = tmp_jackpot_amounts[i];
        }

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomBroadcast::BuildLinkProgressiveCurrentAmountBroadcast()
    {
        if (auto it = m_qcom.lock())
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

            QcomPollPtr poll = this->MakeLinkProgressiveCurrentAmountBroadcast();

            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

            job->AddBroadcast(poll);

            it->AddJob(job);
        }
    }

    //General Promotional Message broadcast
    QcomPollPtr QcomBroadcast::MakeGeneralPromotionalMessageBroadcast(u8 gpm_text_length, const char * gpm_text)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;

        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        poll->poll.Data.Broadcast.ESIZ = sizeof(qc_gpmtype);
        poll->poll.Data.Broadcast.extd.EXTD.gpm.EFUNC = QCOM_BMGPM_FC;
        uint8_t flag = gpm_text_length?1:0;
        poll->poll.Data.Broadcast.extd.EXTD.gpm.FMT.bits.chime = flag;
        if(flag)
        {
            poll->poll.Data.Broadcast.extd.EXTD.gpm.LEN = gpm_text_length;
            strncpy(poll->poll.Data.Broadcast.extd.EXTD.gpm.TEXT, gpm_text, gpm_text_length);
        }
        else
        {
            poll->poll.Data.Broadcast.extd.EXTD.gpm.LEN = 0;
        }
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomBroadcast::BuildGeneralPromotionalMessageBroadcast(u8 gpm_text_length, const char * gpm_text)
    {
        if (auto it = m_qcom.lock())
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

            QcomPollPtr poll = this->MakeGeneralPromotionalMessageBroadcast(gpm_text_length, gpm_text);

            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

            job->AddBroadcast(poll);

            it->AddJob(job);
        }
    }

    //Site Details broadcast
    QcomPollPtr QcomBroadcast::MakeSiteDetailsBroadcast(u8 sds_text_length, u8 sdl_text_length, const char* sds_text, const char* sdl_text)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;

        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        poll->poll.Data.Broadcast.ESIZ = sizeof(qc_sdtype);
        poll->poll.Data.Broadcast.extd.EXTD.sd.EFUNC = QCOM_BMSD_FC;

        poll->poll.Data.Broadcast.extd.EXTD.sd.SLEN = sds_text_length;
        poll->poll.Data.Broadcast.extd.EXTD.sd.LLEN = sdl_text_length;
        strncpy(poll->poll.Data.Broadcast.extd.EXTD.sd.TEXT, sds_text, sds_text_length);
        strncat(poll->poll.Data.Broadcast.extd.EXTD.sd.TEXT, sdl_text, sdl_text_length);

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomBroadcast::BuildSiteDetailsBroadcast(u8 sd_stext_length, u8 sd_ltext_length, const char* sds_text, const char* sdl_text)
    {
        if (auto it = m_qcom.lock())
        {
            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_BROADCAST);

            QcomPollPtr poll = this->MakeSiteDetailsBroadcast(sd_stext_length, sd_ltext_length, sds_text, sdl_text);

            PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

            job->AddBroadcast(poll);

            it->AddJob(job);
        }
    }
}


