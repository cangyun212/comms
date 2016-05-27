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

        ptr = MakeSharedPtr<QcomEGMConfRequestAction>();
        ptr->BuildOptions();
        m_actions["configreq"] = ptr;
        m_actions["cfrq"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfAction>();
        ptr->BuildOptions();
        m_actions["egmconfig"] = ptr;
        m_actions["egmcf"] = ptr;

        //For Broadcast Action
        ptr = MakeSharedPtr<QcomBroadcastAction>();
        ptr->BuildOptions();
        m_actions["broadcast"] = ptr;
        m_actions["bc"] = ptr;

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
    }

}



