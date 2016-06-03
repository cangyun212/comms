#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomGameConfigChange.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg {

    uint8_t QcomGameConfigurationChange::Id() const
    {
        return QCOM_EGMVCP_FC;
    }

    bool QcomGameConfigurationChange::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(buf);
        SG_UNREF_PARAM(length);

        return true;

    }

    void QcomGameConfigurationChange::BuildGameConfigChangePoll(uint8_t poll_address, uint16_t gvn, QcomGameSettingData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            bool invalid_gvn = false;

            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                uint8_t game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;
                uint8_t game = 0;

                for (; game < game_num; ++game)
                {
                    if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY) &&
                        !(p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_REQ))
                        break;
                }

                if (game != game_num)
                {
                    QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);
                    job->AddPoll(this->MakeGameConfigChangePoll(poll_address, p->data.control.last_control, gvn, data));

                    uint8_t var_lock = p->data.games[game].config.settings.var_lock;

                    p->data.games[game].config.settings = data;
                    p->data.games[game].config.settings.var_lock = var_lock;

                    it->AddJob(job);
                }
                else
                {
                    invalid_gvn = true;
                }
            }

            if (invalid_gvn)
            {
                COMMS_LOG(boost::format("EGM poll address %1% has no avaiable game for GVN %2% to be configed currently\n") %
                    static_cast<uint32_t>(poll_address) % gvn, CLL_Error);
            }
        }
    }

    QcomPollPtr QcomGameConfigurationChange::MakeGameConfigChangePoll(uint8_t poll_address, uint8_t last_control, uint16_t gvn,
                                                          QcomGameSettingData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmvcptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMVCP_FC;

        poll->poll.Data.egmvcp.GVN = gvn;
        _QComPutBCD(data.var, &(poll->poll.Data.egmvcp.VAR), sizeof(poll->poll.Data.egmvcp.VAR));
        poll->poll.Data.egmvcp.GFLG.bits.res = (uint8_t)0;
        poll->poll.Data.egmvcp.GFLG.bits.GEF = data.game_enable;
        poll->poll.Data.egmvcp.PGID = data.pgid;

        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
