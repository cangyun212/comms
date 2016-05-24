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
		CmdCompletion::Instance().initCommand("seek");
		CmdCompletion::Instance().initCommand("sk");

        ptr = MakeSharedPtr<QcomEGMPollAddConfAction>();
        m_actions["polladdress"] = ptr;
        m_actions["pa"] = ptr;
		CmdCompletion::Instance().initCommand("polladdress");
		CmdCompletion::Instance().initCommand("pa");

        ptr = MakeSharedPtr<QcomEGMConfRequestAction>();
        m_actions["configreq"] = ptr;
        m_actions["cfrq"] = ptr;
		CmdCompletion::Instance().initCommand("configreq", ptr->GetOptions());
        CmdCompletion::Instance().initCommand("cfrq", ptr->GetOptions());

        ptr = MakeSharedPtr<QcomEGMConfAction>();
        m_actions["egmconfig"] = ptr;
        m_actions["egmcf"] = ptr;
		CmdCompletion::Instance().initCommand("egmconfig", ptr->GetOptions());
		CmdCompletion::Instance().initCommand("egmcf", ptr->GetOptions());

        //For Broadcast Action
        ptr = MakeSharedPtr<QcomBroadcastAction>();
        m_actions["broadcast"] = ptr;
        m_actions["bc"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationAction>();
        m_actions["gameconfig"] = ptr;
        m_actions["gconf"] = ptr;
		CmdCompletion::Instance().initCommand("gameconfig", ptr->GetOptions());
		CmdCompletion::Instance().initCommand("gconf", ptr->GetOptions());

		ptr = MakeSharedPtr<QcomGameConfigurationChangeAction>();
        m_actions["gameconfigchange"] = ptr;
        m_actions["gconfchg"] = ptr;
        CmdCompletion::Instance().initCommand("gameconfigchange", ptr->GetOptions());
        CmdCompletion::Instance().initCommand("gconfchg", ptr->GetOptions());

        ptr = MakeSharedPtr<QcomEGMParametersAction>();
        m_actions["egmparameters"] = ptr;
        m_actions["egmparams"] = ptr;
        CmdCompletion::Instance().initCommand("egmparameters", ptr->GetOptions());
        CmdCompletion::Instance().initCommand("egmparams", ptr->GetOptions());


        ptr = MakeSharedPtr<QcomPurgeEventsAction>();
        m_actions["purgeevents"] = ptr;
        m_actions["pevents"] = ptr;
        CmdCompletion::Instance().initCommand("purgeevents", ptr->GetOptions());
        CmdCompletion::Instance().initCommand("pevents", ptr->GetOptions());
    }

}



