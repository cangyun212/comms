#include "Core.hpp"

#include <string>

#include "Utils.hpp"
#include "Console/ConsoleTable.hpp"
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
                p->MAXDEN(),
                p->MINRTP(),
                p->MAXRTP(),
                p->MAXSD(),
                p->MAXLINES(),
                p->MAXBET(),
                p->MAXNPWIN(),
                p->MAXPWIN(),
                p->MAXECT(),
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

            QcomGameConfigPollData data;

            data.settings.pgid = p->PGID();
            data.settings.var = p->VAR();
            data.settings.var_lock = p->VAR_LOCK();
            data.settings.game_enable = p->GAME_ENABLE();
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

            QcomGameSettingData data{ p->PGID(), p->VAR(), 0, p->GAME_ENABLE() };

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
                p->RESERVE(), 
                p->AUTOPLAY(), 
                p->CRLIMITMODE(), 
                p->OPR(), 
                p->LWIN(), 
                p->CRLIMIT(), 
                p->DULIMIT(), 
                p->DUMAX(),
                p->TZADJ(), 
                p->PID(), 
                p->PWRTIME(), 
                p->NPWINP(), 
                p->SAPWINP(),
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


