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

        ptr = MakeSharedPtr<QcomEGMPollAddConfAction>();
        m_actions["polladdress"] = ptr;
        m_actions["pa"] = ptr;

        ptr = MakeSharedPtr<QcomTimeDateAction>();
        m_actions["timedate"] = ptr;
        m_actions["td"] = ptr;

        ptr = MakeSharedPtr<QcomLPCurrentAmountAction>();
        m_actions["lpcamt"] = ptr;
        m_actions["lp"] = ptr;

        ptr = MakeSharedPtr<QcomGeneralPromotionalAction>();
        m_actions["generalpromt"] = ptr;
        m_actions["gp"] = ptr;

        ptr = MakeSharedPtr<QcomSiteDetailAction>();
        m_actions["sitedetail"] = ptr;
        m_actions["sd"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfRequestAction>();
        ptr->BuildOptions();
        m_actions["configreq"] = ptr;
        m_actions["cfrq"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfAction>();
        ptr->BuildOptions();
        m_actions["egmconfig"] = ptr;
        m_actions["egmcf"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationAction>();
        ptr->BuildOptions();
        m_actions["gameconfig"] = ptr;
        m_actions["gconf"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationChangeAction>();
        ptr->BuildOptions();
        m_actions["gameconfigchange"] = ptr;
        m_actions["gconfchg"] = ptr;

        ptr = MakeSharedPtr<QcomEGMParametersAction>();
        ptr->BuildOptions();
        m_actions["egmparameters"] = ptr;
        m_actions["egmparams"] = ptr;

        ptr = MakeSharedPtr<QcomPurgeEventsAction>();
        ptr->BuildOptions();
        m_actions["purgeevents"] = ptr;
        m_actions["pevents"] = ptr;

        ptr = MakeSharedPtr<QcomEGMGeneralStatusAction>();
        m_actions["genstatus"] = ptr;
        m_actions["gss"] = ptr;

        ptr = MakeSharedPtr<QcomProgressiveConfigAction>();
        ptr->BuildOptions();
        m_actions["progressive"] = ptr;
        m_actions["prog"] = ptr;

        ptr = MakeSharedPtr<QcomExtJPInfoAction>();
        ptr->BuildOptions();
        m_actions["extjp"] = ptr;

        ptr = MakeSharedPtr<QcomPendingAction>();
        ptr->BuildOptions();
        m_actions["pending"] = ptr;

        ptr = MakeSharedPtr<QcomSendAction>();
        m_actions["send"] = ptr;
    }

}



