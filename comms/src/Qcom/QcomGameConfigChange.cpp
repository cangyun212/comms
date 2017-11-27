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

    bool QcomGameConfigurationChange::BuildGameConfigChangePoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomGameSettingData const & data, uint16_t &old_pgid, uint8_t &shared)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            uint8_t game_num = 0;
            uint8_t game = 0;
            bool istate = false;

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                game_num = p->data.config.games_num > 0 ? p->data.config.games_num : QCOM_MAX_GAME_NUM;

                for (; game < game_num; ++game)
                {
                    if ((p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                        (p->data.games[game].gvn == gvn))
                    {
                        if (p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY)
                        {
                            job->AddPoll(this->MakeGameConfigChangePoll(poll_address, p->data.control.last_control, gvn, data));

                            p->data.games[game].config.settings.var = data.var; // set here, verified by request Game Configuration Response
                            p->data.games[game].config.settings.game_enable = data.game_enable;

                            old_pgid = p->data.games[game].config.settings.pgid;
                            if (!p->data.config.shared_progressive)
                            {
                                shared = 0;
                                p->data.games[game].config.settings.pgid = data.pgid;
                            }
                            else
                            {
                                shared = 1;
                                for (uint8_t i = 0; i < game_num; ++i)
                                {
                                    if ((p->data.control.game_config_state[i] & QCOM_GAME_CONFIG_GVN) &&
                                        (p->data.control.game_config_state[i] & QCOM_GAME_CONFIG_READY))
                                    {
                                        p->data.games[i].config.settings.pgid = data.pgid;
                                    }
                                }
                            }
                        }
                        else
                        {
                            istate = true;
                        }

                        break;
                    }
                }
            }

            if (game >= game_num)
            {
                COMMS_LOG(
                    boost::format("Can't change game configuration. GVN 0x%|04X| is not valid or EGM %|| is not configured\n") %
                    gvn % static_cast<uint32_t>(poll_address), CLL_Error);

                return false;
            }

            if (istate)
            {
                COMMS_LOG(
                    boost::format("Can't change game configuration. Game(GVN 0x%|04X|) is not configured or configuration is not verified\n") % 
                    gvn, CLL_Error);

                return false;
            }

            return true;
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
