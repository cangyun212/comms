#include "Core.hpp"
#include "Utils.hpp"

#include <chrono>

#include "Qcom/QcomEgmGeneralMaintenance.hpp"
#include "Qcom/QcomInline.h"

namespace sg
{
    uint8_t QcomEgmGeneralMaintenance::Id() const
    {
        return QCOM_EGMGMP_FC;
    }

    bool QcomEgmGeneralMaintenance::BuildEgmGeneralMaintenancePoll(QcomJobDataPtr &job, uint8_t poll_address, uint16_t gvn, QcomEGMGeneralMaintenancePollData const& data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);

            bool istate = false;
            uint8_t igame = QCOM_MAX_GAME_NUM;

            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeEgmGeneralMaintenancePoll(poll_address, gvn, p->data.control.last_control, data));

                p->data.control.machine_eable = data.mef;

                uint8_t game = 0;
                for (game = 0; game < p->data.config.games_num; ++game)
                {
                    if (p->data.games[game].gvn == gvn)
                    {
                        if (p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY ||
                            p->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET)
                        {
                            p->data.games[game].config.settings.game_enable = data.gef;
                            break;
                        }
                        else
                        {
                            igame = game;
                            istate = true;
                            break;
                        }
                    }
                }

                if (game >= p->data.config.games_num)
                    igame = p->data.config.games_num;
            }

            if (igame != QCOM_MAX_GAME_NUM)
            {
                if (istate)
                {
                    COMMS_LOG(boost::format("Game(GVN:0x%|04X|) is not configured via Game Configuration Poll, GEF flag is ignored\n") % gvn , CLL_Error);
                }
                else
                {
                    COMMS_LOG(boost::format("Game(GVN:0x%|04X|) is not found, GEF flag setting can't be executed\n") % gvn, CLL_Error);
                }
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomEgmGeneralMaintenance::MakeEgmGeneralMaintenancePoll(uint8_t poll_address, uint16_t gvn, uint8_t last_control, QcomEGMGeneralMaintenancePollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMGMP_FC;

        poll->poll.Data.egmgmp.BLK |= (data.meter_group_0 ? 0x1 : 0x0);
        poll->poll.Data.egmgmp.BLK |= (data.meter_group_1 ? (0x1 << 1) : 0x0);
        poll->poll.Data.egmgmp.BLK |= (data.meter_group_2 ? (0x1 << 2) : 0x0);

        poll->poll.Data.egmgmp.FLG.bits.nasr = data.note_acceptor_status;
        poll->poll.Data.egmgmp.FLG.bits.MEF = data.mef;
        
        poll->poll.Data.egmgmp.GVN = gvn;
        _QComPutBCD(data.var, &poll->poll.Data.egmgmp.VAR, sizeof(poll->poll.Data.egmgmp.VAR));

        poll->poll.Data.egmgmp.GFLG.bits.qPCMR = data.player_choice_meter;
        poll->poll.Data.egmgmp.GFLG.bits.qBMR = data.bet_meters;
        poll->poll.Data.egmgmp.GFLG.bits.qPCR = data.progconfig;
        poll->poll.Data.egmgmp.GFLG.bits.qGmeCfg = data.gameconfig;
        poll->poll.Data.egmgmp.GFLG.bits.qLP = data.progmeters;
        poll->poll.Data.egmgmp.GFLG.bits.qSAP = data.progmeters;
        poll->poll.Data.egmgmp.GFLG.bits.qmgmeters = data.multigame;
        poll->poll.Data.egmgmp.GFLG.bits.GEF = data.gef;

        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmgmptype);

        return poll;
    }
}


