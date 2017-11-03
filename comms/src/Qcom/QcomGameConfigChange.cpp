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

    bool QcomGameConfigurationChange::BuildGameConfigChangePoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameSettingData const & data)
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

                // different with GameConfigPoll, we only need to find one available game
                if (!gvn)
                {
                    for (; game < game_num; ++game)
                    {
                        if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY) &&
                            !(p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_REQ))
                            break;
                    }
                }
                else
                {
                    for (; game < game_num; ++game)
                    {
                        if (p->data.games[game].gvn == gvn)
                            break;
                    }
                }

                if (game != game_num)
                {
                    job->AddPoll(this->MakeGameConfigChangePoll(poll_address, p->data.control.last_control, gvn, data));

                    uint8_t var_lock = p->data.games[game].config.settings.var_lock;
                    if (!p->data.games[game].config.settings.var_lock)
                    {
                        p->data.games[game].config.settings.var = data.var;
                    }

                    p->data.games[game].config.settings.game_enable = data.game_enable;

                    if (!gvn)
                    {
                        for (uint8_t i = 0; i < game_num; ++i)
                        {
                            if ((p->data.control.game_config_state[i] & QCOM_GAME_CONFIG_READY) &&
                                p->data.games[i].config.settings.pgid == p->data.games[game].config.settings.pgid)
                            {
                                p->data.games[i].config.settings.pgid = data.pgid;
                            }
                        }
                    }
                    else
                    {
                        p->data.games[game].config.settings.pgid = data.pgid;
                    }

                    // TODO : according Qcom1.6.15.4.4, the EGM will ignore this poll's entire message data are if either
                    // VAR, GVN or PGID field is incorrect or not applicable to the game or EGM. so we may need to check
                    // the data correctiness before we change egm data since there no response for this poll to tell us
                    // if the change was applied.

                    //p->data.games[game].config.settings = data;
                    //p->data.games[game].config.settings.var_lock = var_lock;

                    return true;

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

        return false;
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

        //PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        //poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }
}
