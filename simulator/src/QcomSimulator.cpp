#include "Core.hpp"

#include <string>

#include "Utils.hpp"
#include "Console/ConsoleTable.hpp"
#include "BaseInteger.hpp"
#include "Qcom/Qcom.hpp"
#include "CmdParser.hpp"
#include "Action.hpp"
#include "QcomAction.hpp"
#include "QcomSimulator.hpp"

namespace sg 
{

    QcomSim::QcomSim()
        : m_curr_egm(0)
    {

    }

    QcomSim::~QcomSim()
    {

    }

    void QcomSim::DoInit()
    {
        std::string dev = CmdParser::Instance().GetDevPath();
        if (dev.empty())
        {
#ifdef SG_PLATFORM_LINUX
            dev = "/dev/ttyS0";
#else
            dev = "COM1";
#endif // SG_PLATFORM_LINUX
        }

        m_qcom = MakeSharedPtr<CommsQcom>(dev);
        m_qcom->Init();
        m_qcom->Start();
    }

    void QcomSim::SeekEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->SeekEGM();

        COMMS_LOG("Seek EGM Done [Note: EGM will not always response the seek command if it has responsed one]\n", CLL_Info);
    }

    bool QcomSim::Pick(uint8_t target)
    {
        size_t num = m_qcom->GetEgmNum();

        if (target > 0 && target <= num)
        {
            m_curr_egm = target;
            COMMS_LOG(boost::format("Pick EGM %d\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
            return true;
        }
        else if (target == 0 && num == 1)
        {
            m_curr_egm = 1;
            COMMS_LOG(boost::format("Auto pick EGM %d\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
            return true;
        }

        return false;
    }

    void QcomSim::PickEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        PickEGMActionPtr p = std::static_pointer_cast<PickEGMAction>(action);

        this->Pick(p->Target());
    }

    void QcomSim::ListEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        ListEGMActionPtr p = std::static_pointer_cast<ListEGMAction>(action);

        this->ListEGMInfo(p->ListAll());
    }

    void QcomSim::ListEGMInfo(bool show_all)
    {
        SG_UNREF_PARAM(show_all); // TODO

        std::vector<QcomDataPtr> data;
        m_qcom->CaptureEGMData(data);

        ConsoleTable t;
        t.SetStyle("compact");
        ConsoleTableItem header[] = 
        {
            std::string("EGM"), 
            std::string("SERMID"),
            std::string("JUR"),
            std::string("DEN"),
            std::string("TOK"),
            std::string("MAXDEN"),
            std::string("MINRTP"),
            std::string("MAXRTP"), 
            std::string("MAXSD"), 
            std::string("MAXLINES"), 
            std::string("MAXBET"), 
            std::string("MAXNPWIN"), 
            std::string("MAXPWIN"), 
            std::string("MAXECT")
        };

        t.SetHeader(&header);

        t.SetHeaderCellFormat(1, ConsoleTableFormat(" %|#x| ", CTPT_AlignLeft));

        for (size_t i = 0; i < data.size(); ++i)
        {
            std::unique_lock<std::mutex> lock(data[i]->locker);

            ConsoleTableItem row[] = {
                (i + 1),
                data[i]->data.control.serialMidBCD,
                (uint32_t)data[i]->data.custom.jur,
                data[i]->data.custom.den ,
                data[i]->data.custom.tok ,
                data[i]->data.custom.maxden ,
                data[i]->data.custom.minrtp ,
                data[i]->data.custom.maxrtp ,
                data[i]->data.custom.maxsd ,
                data[i]->data.custom.maxlines ,
                data[i]->data.custom.maxbet ,
                data[i]->data.custom.maxnpwin ,
                data[i]->data.custom.maxpwin ,
                data[i]->data.custom.maxect
            };

            t.AddRow(&row);
        }

        COMMS_START_PRINT_BLOCK();
        COMMS_PRINT_BLOCK("\n");
        COMMS_PRINT_BLOCK(t);
        COMMS_PRINT_BLOCK("\n");
        COMMS_END_PRINT_BLOCK();
    }

    void QcomSim::EGMPollAddConf(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->PollAddress(m_curr_egm);
        m_qcom->GeneralStatus(m_curr_egm);

        if (m_curr_egm)
        {
            COMMS_LOG(boost::format("Config the poll address of EGM %1%\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
        }
        else
        {
            COMMS_LOG("Config the poll address for all EGM\n", CLL_Info);
        }
    }

    void QcomSim::TimeDate(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->TimeData();
    }

    void QcomSim::LPCurrentAmount(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomLPCurrentAmountActionPtr p = std::static_pointer_cast<QcomLPCurrentAmountAction>(action);

        QcomLinkedProgressiveData data;

        data.pnum = p->LPData(data.lpamt, data.pgid, data.plvl);

        m_qcom->LinkJPCurrentAmount(data);
    }

    void QcomSim::GeneralPromotional(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomGeneralPromotionalActionPtr p = std::static_pointer_cast<QcomGeneralPromotionalAction>(action);

        m_qcom->GeneralPromotional(p->Text());
    }

    void QcomSim::SiteDetail(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomSiteDetailActionPtr p = std::static_pointer_cast<QcomSiteDetailAction>(action);

        m_qcom->SiteDetail(p->SText(), p->LText());
    }

    void QcomSim::EGMConfRequest(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfRequestActionPtr p = std::static_pointer_cast<QcomEGMConfRequestAction>(action);

            QcomEGMControlPollData data{ p->MEF(), p->GCR(), p->PSN() };

            m_qcom->EGMConfRequest(m_curr_egm, data);
        }
    }

    void QcomSim::EGMConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfActionPtr p = std::static_pointer_cast<QcomEGMConfAction>(action);

            QcomEGMConfigPollData data
            {
                p->DEN(),
                p->TOK(),
                p->MaxDEN(),
                p->MinRTP(),
                p->MaxRTP(),
                p->MaxSD(),
                p->MaxLines(),
                p->MaxBet(),
                p->MaxNPWin(),
                p->MaxPWin(),
                p->MaxECT(),
                p->JUR()
            };

            m_qcom->EGMConfiguration(m_curr_egm, data);
        }
    }

    void QcomSim::GameConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGameConfigurationActionPtr p = std::static_pointer_cast<QcomGameConfigurationAction>(action);

            QcomGameConfigData data;

            data.settings.pgid = p->PGID();
            data.settings.var = p->VAR();
            data.settings.var_lock = p->VARLock();
            data.settings.game_enable = p->GameEnable();
            data.progressive_config.pnum = p->ProgressiveConfig(data.progressive_config.flag_p, data.progressive_config.camt);

            m_qcom->GameConfiguration(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::GameConfigurationChange(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGameConfigurationChangeActionPtr p = std::static_pointer_cast<QcomGameConfigurationChangeAction>(action);

            QcomGameSettingData data{ p->PGID(), p->VAR(), 0, p->GameEnable() };

            m_qcom->GameConfigurationChange(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::EGMParameters(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMParametersActionPtr p = std::static_pointer_cast<QcomEGMParametersAction>(action);

            QcomEGMParametersData data 
            {
                p->Reserve(), 
                p->AutoPlay(), 
                p->CRLimitMode(), 
                p->OPR(), 
                p->LWin(), 
                p->CRLimit(), 
                p->DULimit(), 
                p->DUMax(),
                p->TZADJ(), 
                p->PID(), 
                p->PWRTime(), 
                p->NPWinP(), 
                p->SAPWinP(),
                p->EODT()
            };

            m_qcom->EGMParameters(m_curr_egm, data);
        }
    }

    void QcomSim::PurgeEvents(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomPurgeEventsActionPtr p = std::static_pointer_cast<QcomPurgeEventsAction>(action);

            m_qcom->PurgeEvents(m_curr_egm, p->EVTNO());
        }
    }

    void QcomSim::GeneralStatus(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->GeneralStatus(m_curr_egm);

    }

    void QcomSim::ProgressiveChange(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomProgressiveConfigActionPtr p = std::static_pointer_cast<QcomProgressiveConfigAction>(action);

            QcomProgressiveChangeData data;
            data.pnum = p->ProgChangeData(data.sup, data.prog.pinc, data.prog.ceil, data.prog.auxrtp);
            
            m_qcom->ProgressiveChange(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::ExtJPInfo(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomExtJPInfoActionPtr p = std::static_pointer_cast<QcomExtJPInfoAction>(action);

            QcomExtJPInfoData data;
            data.levels = p->ExtJPData(data.epgid, data.lumf, data.lname);

            data.rtp = p->ExtJPRTP();
            data.display = p->ExtJPDisplay();
            data.icon = p->ExtJPIcon();

            m_qcom->ExtJPInfo(m_curr_egm, data);
        }
    }

    void QcomSim::ProgHashRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomProgHashRequestActionPtr p = std::static_pointer_cast<QcomProgHashRequestAction>(action);

            QcomProgHashRequestData data;
            data.mef = p->MEF();
            data.new_seed = p->Seed(data.seed, sizeof(data.seed));

            m_qcom->ProgHashRequest(m_curr_egm, data);
        }
    }

    void QcomSim::SysLockupRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomSysLockupRequestActionPtr p = std::static_pointer_cast<QcomSysLockupRequestAction>(action);

            QcomSysLockupRequestData data;

            std::string text = p->Text();
            data.len = static_cast<uint8_t>(text.size());
            data.len = data.len <= QCOM_SALRP_TEXT_SIZE ? data.len : QCOM_SALRP_TEXT_SIZE;
            std::memcpy(data.text, text.c_str(), data.len);

            data.no_resetkey = p->NoResetKey();
            data.continue_style = p->ContinueStyle();
            data.question_style = p->QuestionStyle();
            data.lamp_test = p->LampTest();
            data.fanfare = p->Fanfare();

            m_qcom->SystemLockup(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketOutAck(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomCashTicketOutAckActionPtr p = std::static_pointer_cast<QcomCashTicketOutAckAction>(action);

            QcomCashTicketOutRequestAckPollData data;

            std::string certification = p->CertificationMessage();
            data.clen = static_cast<uint8_t>(certification.size());
            data.clen = data.clen <= QCOM_TORAP_MAX_CTEXT ? data.clen : QCOM_TORAP_MAX_CTEXT;
            std::memcpy(data.certification, certification.c_str(), data.clen);

            data.amount = p->Amount();
            data.serial = p->Serial();

            data.flag = 0;
            if (p->Approved())
                data.flag &= QCOM_CTO_APPROVE;
            if (p->Canceled())
                data.flag &= QCOM_CTO_CANCEL;

            BaseDecimal dec;
            DecimalInteger authno(p->AuthorisationNumber(), dec);
            HexInteger hex(authno);

            uint8_t digit = 0;
            size_t i = 0;
            for (; i < hex.GetCounts() && ((i / 2) < QCOM_AUTHNO_BYTE_NUM); ++i)
            {
                if (i % 2) // high bit
                {
                    digit &= (hex.GetDigit(i) << 4);
                    data.authno[i / 2] = digit;
                    digit = 0;
                }
                else
                {
                    digit = hex.GetDigit(i);
                }
            }

            if (i % 2 && ((i / 2) < QCOM_AUTHNO_BYTE_NUM)) // hex counts is odd
            {
                data.authno[i / 2] = digit;
            }

            for (size_t j = (i / 2); j < QCOM_AUTHNO_BYTE_NUM; ++j)
            {
                data.authno[j] = 0;
            }

            m_qcom->CashTicketOutAck(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketInAck(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomCashTicketInAckActionPtr p = std::static_pointer_cast<QcomCashTicketInAckAction>(action);

            QcomCashTicketInRequestAckPollData data;

            data.amount = p->Amount();
            data.fcode = p->FCode();

            BaseDecimal dec;
            DecimalInteger authno(p->AuthorisationNumber(), dec);
            HexInteger hex(authno);

            uint8_t digit = 0;
            size_t i = 0;
            for (; i < hex.GetCounts() && ((i / 2) < QCOM_AUTHNO_BYTE_NUM); ++i)
            {
                if (i % 2) // high bit
                {
                    digit &= (hex.GetDigit(i) << 4);
                    data.authno[i / 2] = digit;
                    digit = 0;
                }
                else
                {
                    digit = hex.GetDigit(i);
                }
            }

            if (i % 2 && ((i / 2) < QCOM_AUTHNO_BYTE_NUM)) // hex counts is odd
            {
                data.authno[i / 2] = digit;
            }

            for (size_t j = (i / 2); j < QCOM_AUTHNO_BYTE_NUM; ++j)
            {
                data.authno[j] = 0;
            }

            m_qcom->CashTicketInAck(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketOutRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->CashTicketOutReqeust(m_curr_egm);
        }
    }

    void QcomSim::EGMGeneralMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {

        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMGeneralMaintenanceActionPtr p = std::static_pointer_cast<QcomEGMGeneralMaintenanceAction>(action);

            QcomEGMGeneralMaintenancePollData data;

            data.meter_group_0 = p->MetersGroupFlag(0);
            data.meter_group_1 = p->MetersGroupFlag(1);
            data.meter_group_2 = p->MetersGroupFlag(2);
            data.note_acceptor_status = p->Qnasr();
            data.mef = p->MEF();
            data.var = p->VAR();
            data.player_choice_meter = p->Qpcmr();
            data.bet_meters = p->Qbmr();
            data.progconfig = p->Qprogcfg();
            data.gameconfig = p->Qgmecfg();
            data.progmeters = p->Qprogmeters();
            data.multigame = p->Qmultigame();
            data.gef = p->GEF();

            m_qcom->EGMGeneralMaintenance(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::RequestAllLoggedEvents(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->RequestAllLoggedEvents(m_curr_egm);
        }
    }

    void QcomSim::NoteAcceptorMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomNoteAcceptorMaintenanceActionPtr p = std::static_pointer_cast<QcomNoteAcceptorMaintenanceAction>(action);

            QcomNoteAcceptorMaintenanceData data;
            data.five = p->GetDenomFlag(5);
            data.ten = p->GetDenomFlag(10);
            data.twenty = p->GetDenomFlag(20);
            data.fifty = p->GetDenomFlag(50);
            data.hundred = p->GetDenomFlag(100);

            m_qcom->NoteAcceptorMaintenance(m_curr_egm, data);
        }
    }

    void QcomSim::HopperTicketPrinterMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomHopperTicketPrinterMaintenanceActionPtr p = std::static_pointer_cast<QcomHopperTicketPrinterMaintenanceAction>(action);

            QcomHopperTicketPrinterData data;
            data.refill = p->Refill();
            data.collim = p->COLLIM();
            data.ticket = p->Ticket();
            data.dorefill = p->DoRefill();

            m_qcom->HopperTicketPrinterMaintenance(m_curr_egm, p->Test(), data);
        }
    }

    void QcomSim::LPAwardAck(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->LPAwardAck(m_curr_egm);
        }
    }

    void QcomSim::GeneralReset(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGeneralResetActionPtr p = std::static_pointer_cast<QcomGeneralResetAction>(action);

            QcomGeneralResetPollData data;
            data.fault = p->Fault();
            data.lockup = p->Lockup();
            data.state = p->State();

            m_qcom->GeneralReset(m_curr_egm, data);
        }
    }

    void QcomSim::PendingPoll(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomPendingActionPtr p = std::static_pointer_cast<QcomPendingAction>(action);

        m_qcom->PendingPoll(p->PollNum());
    }

    void QcomSim::SendPoll(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->SendPoll();
    }

    void QcomSim::ChangeDev(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        ResetDevActionPtr a = std::static_pointer_cast<ResetDevAction>(action);
        m_qcom->ChangeDev(a->GetDev());
    }
}


