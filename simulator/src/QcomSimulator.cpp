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
            dev = "/dev/ptmx";
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
        m_qcom->GetEgmData(data);

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
                data[i]->data.serialMidBCD,
                (uint32_t)data[i]->data.egm_config.jur,
                data[i]->data.egm_config.den ,
                data[i]->data.egm_config.tok ,
                data[i]->data.egm_config.maxden ,
                data[i]->data.egm_config.minrtp ,
                data[i]->data.egm_config.maxrtp ,
                data[i]->data.egm_config.maxsd ,
                data[i]->data.egm_config.maxlines ,
                data[i]->data.egm_config.maxbet ,
                data[i]->data.egm_config.maxnpwin ,
                data[i]->data.egm_config.maxpwin ,
                data[i]->data.egm_config.maxect
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
        COMMS_LOG(boost::format("Config the poll address of EGM %1%\n") % static_cast<uint32_t>(m_curr_egm + 1), CLL_Info);
    }

    void QcomSim::EGMConfRequest(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfRequestActionPtr p = std::static_pointer_cast<QcomEGMConfRequestAction>(action);

            m_qcom->EGMConfRequest(m_curr_egm, p->MEF(), p->GCR(), p->PSN());
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

            m_qcom->EGMConfiguration(m_curr_egm, 
                p->JUR(), 
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
                p->MAXECT());
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

            std::vector<uint8_t> lp;
            std::vector<uint32_t> camt;

            p->LP(lp);
            p->CAMT(camt);

            m_qcom->GameConfiguration(m_curr_egm, 
                p->VAR(), 
                p->VAR_LOCK(), 
                p->GAME_ENABLE(), 
                p->PNMUM(), 
                lp, 
                camt);
        }
    }



    void QcomSim::SendBroadcast(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        //if (m_curr_egm == 0)
        //    Pick(0);

        //if (m_curr_egm > 0)
        {
            QcomBroadcastActionPtr p = std::static_pointer_cast<QcomBroadcastAction>(action);

            m_qcom->SendBroadcast(
                p->GetBroadcastType(), 
                p->GetGPMBroadcastText(), 
                p->GetSDSBroadcastText(), 
                p->GetSDLBroadcastText());
        }
    }

    void QcomSim::ChangeDev(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        ResetDevActionPtr a = std::static_pointer_cast<ResetDevAction>(action);
        m_qcom->ChangeDev(a->GetDev());
    }
}


