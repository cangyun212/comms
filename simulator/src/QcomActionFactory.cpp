#include "Core.hpp"
#include "Utils.hpp"

#include "QcomAction.hpp"
#include "QcomActionFactory.hpp"


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
        m_actions["configreq"] = ptr;
        m_actions["cfrq"] = ptr;

        ptr = MakeSharedPtr<QcomEGMConfAction>();
        m_actions["egmconfig"] = ptr;
        m_actions["egmcf"] = ptr;

        //For Broadcast Action
        ptr = MakeSharedPtr<QcomBroadcastAction>();
        m_actions["broadcast"] = ptr;
        m_actions["bc"] = ptr;

        ptr = MakeSharedPtr<QcomGameConfigurationAction>();
        m_actions["gameconfig"] = ptr;
        m_actions["gconf"] = ptr;
    }

}



