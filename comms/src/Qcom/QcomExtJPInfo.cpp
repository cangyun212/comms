#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomExtJPInfo.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg
{
    uint8_t QcomExtJPInfo::Id() const
    {
        return QCOM_EXTJIP_FC;
    }

    bool QcomExtJPInfo::BuildExtJPInfoPoll(QcomJobDataPtr & job, uint8_t poll_address, QcomExtJPInfoData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeExtJPInfoPoll(poll_address, p->data.control.last_control, data));

                p->data.extjpinfo = data;

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomExtJPInfo::MakeExtJPInfoPoll(uint8_t poll_address, uint8_t last_control, QcomExtJPInfoData const & data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_extjiptype) - sizeof(poll->poll.Data.extjip.re);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EXTJIP_FC;

        poll->poll.Data.extjip.RES = 0;
        poll->poll.Data.extjip.RTP = data.rtp;
        poll->poll.Data.extjip.SIZ = sizeof(qc_extjipretype);
        poll->poll.Data.extjip.FLG.bits.num = data.levels;
        poll->poll.Data.extjip.FLG.bits.display = data.display;
        poll->poll.Data.extjip.FLG.bits.res2 = data.icon;

        if (data.rtp)
        {
            poll->poll.DLL.Length += (data.levels * poll->poll.Data.extjip.SIZ);

            for (uint8_t i = 0; i < data.levels; ++i)
            {
                qc_extjipretype *ent = (qc_extjipretype*)((uint8_t*)poll->poll.Data.extjip.re + i * poll->poll.Data.extjip.SIZ);

                ent->PGID = data.epgid[i];
                ent->LFLG.bits.lev = i;
                ent->LFLG.LFLG |= (data.lumf[i] ? 0x8000 : 0);

                std::strcpy(ent->LNAME, data.lname[i]);
            }
        }

        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}


