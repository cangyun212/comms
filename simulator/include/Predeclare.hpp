#ifndef __SG_PREDECLARE_HPP__
#define __SG_PREDECLARE_HPP__

namespace sg {

    class Action;
    class ActionCenter;
    class QuitAction;
    class ResetDevAction;
    class ListEGMAction;
    class PickEGMAction;
    class QcomSeekEGMAction;
    class QcomEGMPollAddConfAction;
    class QcomEGMConfRequestAction;
    class QcomEGMConfAction;
    class QcomGameConfigurationAction;
    class ActionFactory;
    class QcomActionFactory;
    class Simulator;
    class QcomSim;
    class QcomBroadcastAction;
    
    typedef std::shared_ptr<Simulator>   SimulatorPtr;
    typedef std::shared_ptr<QcomSim>     QcomSimPtr;
    typedef std::shared_ptr<Action>      ActionPtr;
    typedef std::shared_ptr<QuitAction>  QuitActionPtr;
    typedef std::shared_ptr<ResetDevAction>  ResetDevActionPtr;
    typedef std::shared_ptr<ListEGMAction>   ListEGMActionPtr;
    typedef std::shared_ptr<PickEGMAction>   PickEGMActionPtr;
    typedef std::shared_ptr<ActionFactory>   ActionFactoryPtr;
    typedef std::shared_ptr<QcomActionFactory> QcomActionFactoryPtr;
    typedef std::shared_ptr<QcomSeekEGMAction>   QcomSeekEGMActionPtr;
    typedef std::shared_ptr<QcomEGMPollAddConfAction> QcomEGMPollAddConfActionPtr;
    typedef std::shared_ptr<QcomEGMConfRequestAction> QcomEGMConfRequestActionPtr;
    typedef std::shared_ptr<QcomEGMConfAction> QcomEGMConfActionPtr;
    typedef std::shared_ptr<QcomGameConfigurationAction> QcomGameConfigurationActionPtr;
    typedef std::shared_ptr<QcomBroadcastAction> QcomBroadcastActionPtr;
}



#endif




