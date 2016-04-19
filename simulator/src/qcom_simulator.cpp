#include "core/core.hpp"
#include "core/core_utils.hpp"
#include "core/console/core_console_printer.hpp"
#include "core/console/core_console_table.hpp"

#include <string>

#include "comms/qcom/qcom.hpp"
#include "simulator/cmd_parser.hpp"
#include "simulator/action.hpp"
#include "simulator/qcom_action.hpp"
#include "simulator/qcom_simulator.hpp"

namespace sg {

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
        CORE_UNREF_PARAM(sender);
        CORE_UNREF_PARAM(action);

        m_qcom->SeekEGM();

        CORE_LOG(LL_Info, "Seek EGM Done [Note: EGM will not always response the seek command if it has responsed one]\n");
    }

    bool QcomSim::Pick(uint8_t target)
    {
        size_t num = m_qcom->GetEgmNum();

        if (target > 0 && target <= num)
        {
            m_curr_egm = target;
            CORE_LOG(LL_Info, boost::format("Pick EGM %d\n") % (uint32_t)m_curr_egm);
            return true;
        }
        else if (target == 0 && num == 1)
        {
            m_curr_egm = num;
            CORE_LOG(LL_Info, boost::format("Auto pick EGM %d\n") % (uint32_t)m_curr_egm);
            return true;
        }

        return false;
    }

    void QcomSim::PickEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        PickEGMActionPtr p = static_pointer_cast<PickEGMAction>(action);

        Pick(p->Target());
    }

    void QcomSim::ListEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        ListEGMActionPtr p = static_pointer_cast<ListEGMAction>(action);

        ListEGMInfo(p->ListAll());
    }

    void QcomSim::ListEGMInfo(bool show_all)
    {
        std::vector<QcomDataPtr> data;
        m_qcom->GetEgmData(data);

        ConsoleTable t;
        t.SetStyle("compact");
        CTableItem header[] = { "EGM", "SERMID", "JUR", "DEN", "TOK", "MAXDEN", "MINRTP", "MAXRTP", "MAXSD", "MAXLINES", "MAXBET", "MAXNPWIN", "MAXPWIN", "MAXECT" };
        t.SetHeader(&header);

        t.SetHeaderCellFormat(1, CTableFormat(" %|#x| ", TPT_AlignLeft));

        for (size_t i = 0; i < data.size(); ++i)
        {
            shared_lock<shared_mutex> lock(data[i]->locker);

            CTableItem row[] = {
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

        CORE_START_PRINT_BLOCK();
        CORE_PRINT_BLOCK("\n");
        CORE_PRINT_BLOCK(t);
        CORE_PRINT_BLOCK("\n");
        CORE_END_PRINT_BLOCK();
    }

    void QcomSim::EGMPollAddConf(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);
        CORE_UNREF_PARAM(action);

        m_qcom->PollAddress(m_curr_egm);
        CORE_LOG(LL_Info, boost::format("Config the poll address of EGM %1%\n") % (uint32_t)(m_curr_egm+1));
    }

    void QcomSim::EGMConfRequest(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfRequestActionPtr p = static_pointer_cast<QcomEGMConfRequestAction>(action);

            m_qcom->EGMConfRequest(m_curr_egm, p->MEF(), p->GCR(), p->PSN());
        }
    }

    void QcomSim::EGMConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfActionPtr p = static_pointer_cast<QcomEGMConfAction>(action);

            m_qcom->EGMConfiguration(m_curr_egm, (uint8_t)p->JUR(), (uint32_t)p->DEN(), (uint32_t)p->TOK(), (uint32_t)p->MAXDEN(), (uint16_t)p->MINRTP(), (uint16_t)p->MAXRTP(), (uint16_t)p->MAXSD(),
                            (uint16_t)p->MAXLINES(), (uint32_t)p->MAXBET(), (uint32_t)p->MAXNPWIN(), (uint32_t)p->MAXPWIN(), (uint32_t)p->MAXECT());
        }
    }

    void QcomSim::GameConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGameConfigurationActionPtr p = static_pointer_cast<QcomGameConfigurationAction>(action);

            std::vector<uint8_t> lp(p->LP().begin(), p->LP().end());
            std::vector<uint32_t> camt(p->CAMT().begin(), p->CAMT().end());

            m_qcom->GameConfiguration(m_curr_egm, (uint8_t)p->VAR(), (uint8_t)p->VAR_LOCK(), (uint8_t)p->GAME_ENABLE(), (uint8_t)p->PNMUM(), lp, camt);
        }
    }



    void QcomSim::SendBroadcast(const ActionCenter &sender, const ActionPtr &action)
    {
        CORE_UNREF_PARAM(sender);

        //if (m_curr_egm == 0)
        //    Pick(0);

        //if (m_curr_egm > 0)
        {
            QcomBroadcastActionPtr p = static_pointer_cast<QcomBroadcastAction>(action);

            m_qcom->SendBroadcast(p->GetBroadcastType(), p->GetGPMBroadcastText(), p->GetSDSBroadcastText(), p->GetSDLBroadcastText());
        }
    }

    void QcomSim::ChangeDev(const ActionCenter &sender, const ActionPtr &action)
    {
        ResetDevActionPtr a = static_pointer_cast<ResetDevAction>(action);
        m_qcom->ChangeDev(a->GetDev());
    }
}


