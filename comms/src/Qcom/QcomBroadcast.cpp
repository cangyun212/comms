 
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

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    QcomPollPtr QcomBroadcast::MakePollAddressPoll(uint32_t ser, uint8_t poll_address)
    {
        QcomPollPtr poll = this->MakePollAddressPoll(1);

        poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[0].SN.SER = ser;
        poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[0].PADR = poll_address;

        return poll;
    }

    bool QcomBroadcast::MakePollAddressJob(QcomJobDataPtr job)
    {
        if (auto it = m_qcom.lock())
        {
            std::vector<QcomDataPtr> egmDatas;
            it->CaptureEGMData(egmDatas);

            size_t size = egmDatas.size();
            if(!size)
                return false;

            // batch by QCOM_REMAX_BMEGMPAC, ref Qcom1.6-15.5.4
            size_t group = size / QCOM_REMAX_BMEGMPAC;
            size_t extra = size % QCOM_REMAX_BMEGMPAC;

            QcomPollPtr poll_extra = this->MakePollAddressPoll(extra);
            for (size_t i = 0; i < extra; ++i)
            {
                std::unique_lock<std::mutex> lock(egmDatas[i]->locker);

                poll_extra->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].SN.SER = egmDatas[i]->data.control.serialMidBCD;
                poll_extra->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].PADR = static_cast<u8>(i + 1);
                egmDatas[i]->data.control.poll_address = static_cast<u8>(i + 1);
            }

            job->AddBroadcast(poll_extra);

            for (size_t j = 0; j < group; ++j)
            {
                QcomPollPtr poll = this->MakePollAddressPoll(QCOM_REMAX_BMEGMPAC);
                size_t low_bound = extra + j * QCOM_REMAX_BMEGMPAC;
                size_t high_bound = low_bound + QCOM_REMAX_BMEGMPAC;
                for (size_t i = low_bound; i < high_bound; ++i)
                {
                    std::unique_lock<std::mutex> lock(egmDatas[i]->locker);

                    poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].SN.SER = egmDatas[i]->data.control.serialMidBCD;
                    poll->poll.Data.Broadcast.extd.EXTD.egmpac.re[i].PADR = static_cast<u8>(i + 1);
                    egmDatas[i]->data.control.poll_address = static_cast<u8>(i + 1);
                }
                job->AddBroadcast(poll);
            }

            return true;
        }

        return false;
    }

    bool QcomBroadcast::BuildPollAddressPoll(QcomJobDataPtr job)
    {
        //if (auto it = m_qcom.lock())
        //{
            return this->MakePollAddressJob(job);
        //}

        //return false;
    }

    bool QcomBroadcast::BuildPollAddressPoll(QcomJobDataPtr job, uint8_t poll_address)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr d = it->GetEgmData(poll_address);

            if (d)
            {
                std::unique_lock<std::mutex> lock(d->locker);

                job->AddBroadcast(this->MakePollAddressPoll(d->data.control.serialMidBCD, poll_address));

                d->data.control.poll_address = poll_address;

                return true;
            }
        }

        return false;
    }

    //Time Data broadcast
    QcomPollPtr QcomBroadcast::MakeTimeDateBroadcast()
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

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    bool QcomBroadcast::BuildTimeDateBroadcast(QcomJobDataPtr job)
    {
        //if (auto it = m_qcom.lock())
        //{
            job->AddBroadcast(this->MakeTimeDateBroadcast());

            return true;
        //}
    }

    //Link Progressive Current Amount broadcast
    QcomPollPtr QcomBroadcast::MakeLinkProgressiveCurrentAmountBroadcast(QcomLinkedProgressiveData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();
        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = QCOM_BROADCAST_ADDRESS;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_broadcastpolltype);
        poll->poll.DLL.ControlByte.CNTL = QCOM_CNTL_POLL_BIT;
        poll->poll.DLL.FunctionCode = QCOM_BROADCAST_POLL_FC;

        poll->poll.Data.Broadcast.FLG.bits.SEF = 1;
        poll->poll.Data.Broadcast.FLG.bits.clock = 1;

        poll->poll.Data.Broadcast.ESIZ = static_cast<u8>(sizeof(qc_lpcatype) + sizeof(qc_lpcaretype) * (data.pnum));
        poll->poll.Data.Broadcast.extd.EXTD.lpca.EFUNC = QCOM_BMLPCA_FC;
        poll->poll.Data.Broadcast.extd.EXTD.lpca.NUM.bits.levels = data.pnum - 1;

        for(size_t i = 0; i < data.pnum; ++i)
        {
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].PGID = data.pgid[i];
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].PLVL.bits.level = i;
            poll->poll.Data.Broadcast.extd.EXTD.lpca.re[i].CAMT = data.lpamt[i];
        }

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    bool QcomBroadcast::BuildLinkProgressiveCurrentAmountBroadcast(QcomJobDataPtr job, QcomLinkedProgressiveData const& data)
    {
        //if (auto it = m_qcom.lock())
        //{
            job->AddBroadcast(this->MakeLinkProgressiveCurrentAmountBroadcast(data));

            return true;
        //}
    }

    //General Promotional Message broadcast
    QcomPollPtr QcomBroadcast::MakeGeneralPromotionalMessageBroadcast(std::string const& text)
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

        uint8_t length = text.size() <= QCOM_BMGPM_TEXT_SIZE ? static_cast<uint8_t>(text.size()) : QCOM_BMGPM_TEXT_SIZE;

        uint8_t flag = length ? 1 : 0;
        poll->poll.Data.Broadcast.extd.EXTD.gpm.FMT.bits.chime = flag;
        if(flag)
        {
            poll->poll.Data.Broadcast.extd.EXTD.gpm.LEN = length;
            strncpy(poll->poll.Data.Broadcast.extd.EXTD.gpm.TEXT, text.c_str(), length);
        }
        else
        {
            poll->poll.Data.Broadcast.extd.EXTD.gpm.LEN = 0;
        }

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    bool QcomBroadcast::BuildGeneralPromotionalMessageBroadcast(QcomJobDataPtr job, std::string const& text)
    {
        //if (auto it = m_qcom.lock())
        //{
            job->AddBroadcast(this->MakeGeneralPromotionalMessageBroadcast(text));

            return true;
        //}
    }

    //Site Details broadcast
    QcomPollPtr QcomBroadcast::MakeSiteDetailsBroadcast(std::string const& stext, std::string const& ltext)
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

        uint8_t slen = stext.size() <= QCOM_BMSD_SLEN ? static_cast<uint8_t>(stext.size()) : QCOM_BMSD_SLEN;
        uint8_t llen = ltext.size() <= QCOM_BMSD_LLEN ? static_cast<uint8_t>(ltext.size()) : QCOM_BMSD_LLEN;

        poll->poll.Data.Broadcast.extd.EXTD.sd.SLEN = slen;
        poll->poll.Data.Broadcast.extd.EXTD.sd.LLEN = llen;
        strncpy(poll->poll.Data.Broadcast.extd.EXTD.sd.TEXT, stext.c_str(), slen);
        strncat(poll->poll.Data.Broadcast.extd.EXTD.sd.TEXT + slen, ltext.c_str(), llen);

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);

        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    bool QcomBroadcast::BuildSiteDetailsBroadcast(QcomJobDataPtr job, std::string const& stext, std::string const& ltext)
    {
        //if (auto it = m_qcom.lock())
        //{
            job->AddBroadcast(this->MakeSiteDetailsBroadcast(stext, ltext));

            return true;
        //}
    }
}


