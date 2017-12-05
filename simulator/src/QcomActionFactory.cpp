#include "Core.hpp"
#include "Utils.hpp"

#include "QcomAction.hpp"
#include "QcomActionFactory.hpp"
#include "CmdCompletion.hpp"


namespace sg 
{

    QcomActionFactory::QcomActionFactory()
    {

    }

    QcomActionFactory::~QcomActionFactory()
    {

    }

    void QcomActionFactory::Init()
    {
        ActionFactory::Init();

        ActionPtr ptr = MakeSharedPtr<QcomSeekEGMAction>();
        m_actions["seek"] = ptr;
        m_actions["sk"] = ptr;

        ptr = MakeSharedPtr<QcomListAction>();
        m_actions["list"] = ptr;
        m_actions["ls"] = ptr;
        m_actions["l"] = ptr;

        ptr = MakeSharedPtr<QcomEGMPollAddConfAction>();
        m_actions["polladdress"] = ptr;
        m_actions["pa"] = ptr;

        ptr = MakeSharedPtr<QcomTimeDateAction>();
        m_actions["timedate"] = ptr;
        m_actions["td"] = ptr;

        ptr = MakeSharedPtr<QcomLPCurrentAmountAction>();
        ptr->BuildOptions();
        m_actions["lpcamt"] = ptr;
        m_actions["lp"] = ptr;

        ptr = MakeSharedPtr<QcomGeneralPromotionalAction>();
        ptr->BuildOptions();
        m_actions["generalpromt"] = ptr;
        m_actions["gp"] = ptr;

        ptr = MakeSharedPtr<QcomSiteDetailAction>();
        ptr->BuildOptions();
        m_actions["sitedetail"] = ptr;
        m_actions["sd"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfRequestAction>();
        ptr->BuildOptions();
        m_actions["configrequest"] = ptr;
        m_actions["cr"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfAction>();
        ptr->BuildOptions();
        m_actions["egmconfig"] = ptr;
        m_actions["cf"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationAction>();
        ptr->BuildOptions();
        m_actions["gameconfig"] = ptr;
        m_actions["gc"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationChangeAction>();
        ptr->BuildOptions();
        m_actions["changegconf"] = ptr;
        m_actions["cc"] = ptr;

        ptr = MakeSharedPtr<QcomEGMParametersAction>();
        ptr->BuildOptions();
        m_actions["egmparams"] = ptr;
        m_actions["pp"] = ptr;

        ptr = MakeSharedPtr<QcomPurgeEventsAction>();
        ptr->BuildOptions();
        m_actions["purgeevents"] = ptr;
        m_actions["pe"] = ptr;

        ptr = MakeSharedPtr<QcomEGMGeneralStatusAction>();
        m_actions["generalstatus"] = ptr;
        m_actions["gs"] = ptr;

        ptr = MakeSharedPtr<QcomProgressiveConfigAction>();
        ptr->BuildOptions();
        m_actions["progressive"] = ptr;
        m_actions["pc"] = ptr;

        ptr = MakeSharedPtr<QcomExtJPInfoAction>();
        ptr->BuildOptions();
        m_actions["extjackpotinfo"] = ptr;
        m_actions["extjpi"] = ptr;

        ptr = MakeSharedPtr<QcomProgHashRequestAction>();
        ptr->BuildOptions();
        m_actions["hash"] = ptr;

        ptr = MakeSharedPtr<QcomSysLockupRequestAction>();
        ptr->BuildOptions();
        m_actions["lockup"] = ptr;

        ptr = MakeSharedPtr<QcomCashTicketOutAckAction>();
        ptr->BuildOptions();
        m_actions["torack"] = ptr;

        ptr = MakeSharedPtr<QcomCashTicketInAckAction>();
        ptr->BuildOptions();
        m_actions["tirack"] = ptr;

        ptr = MakeSharedPtr<QcomCashTicketOutRequestAction>();
        m_actions["reqto"] = ptr;

        ptr = MakeSharedPtr<QcomEGMGeneralMaintenanceAction>();
        ptr->BuildOptions();
        m_actions["generalmainten"] = ptr;
        m_actions["gm"] = ptr;

        ptr = MakeSharedPtr<QcomRequestAllLoggedEventsAction>();
        m_actions["reqevents"] = ptr;

        ptr = MakeSharedPtr<QcomNoteAcceptorMaintenanceAction>();
        ptr->BuildOptions();
        m_actions["nam"] = ptr;

        ptr = MakeSharedPtr<QcomHopperTicketPrinterMaintenanceAction>();
        ptr->BuildOptions();
        m_actions["htpm"] = ptr;

        ptr = MakeSharedPtr<QcomLPAwardAckAction>();
        ptr->BuildOptions();
        m_actions["lpack"] = ptr;
        m_actions["la"] = ptr;

        ptr = MakeSharedPtr<QcomGeneralResetAction>();
        ptr->BuildOptions();
        m_actions["reset"] = ptr;

        ptr = MakeSharedPtr<QcomSPAMAction>();
        ptr->BuildOptions();
        m_actions["spam"] = ptr;

        ptr = MakeSharedPtr<QcomTowerLightMaintenanceAction>();
        ptr->BuildOptions();
        m_actions["towerlight"] = ptr;
        m_actions["tl"] = ptr;

        ptr = MakeSharedPtr<QcomECTToEGMAction>();
        ptr->BuildOptions();
        m_actions["ect2egm"] = ptr;
        m_actions["ete"] = ptr;

        ptr = MakeSharedPtr<QcomECTFromEGMLockupRequestAction>();
        m_actions["ectfromegm"] = ptr;
        m_actions["efe"] = ptr;

        ptr = MakeSharedPtr<QcomECTLockupResetAction>();
        m_actions["ect"] = ptr;

        ptr = MakeSharedPtr<QcomPendingAction>();
        ptr->BuildOptions();
        m_actions["pending"] = ptr;

        ptr = MakeSharedPtr<QcomSendAction>();
        m_actions["send"] = ptr;
    }

}



