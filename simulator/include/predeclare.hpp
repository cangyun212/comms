#ifndef __PREDECLARE_HPP__
#define __PREDECLARE_HPP__

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
    
    typedef shared_ptr<Simulator>   SimulatorPtr;
    typedef shared_ptr<QcomSim>     QcomSimPtr;
    typedef shared_ptr<Action>      ActionPtr;
    typedef shared_ptr<QuitAction>  QuitActionPtr;
    typedef shared_ptr<ResetDevAction>  ResetDevActionPtr;
    typedef shared_ptr<ListEGMAction>   ListEGMActionPtr;
    typedef shared_ptr<PickEGMAction>   PickEGMActionPtr;
    typedef shared_ptr<ActionFactory>   ActionFactoryPtr;
    typedef shared_ptr<QcomActionFactory> QcomActionFactoryPtr;
    typedef shared_ptr<QcomSeekEGMAction>   QcomSeekEGMActionPtr;
    typedef shared_ptr<QcomEGMPollAddConfAction> QcomEGMPollAddConfActionPtr;
    typedef shared_ptr<QcomEGMConfRequestAction> QcomEGMConfRequestActionPtr;
    typedef shared_ptr<QcomEGMConfAction> QcomEGMConfActionPtr;
    typedef shared_ptr<QcomGameConfigurationAction> QcomGameConfigurationActionPtr;
    typedef shared_ptr<QcomBroadcastAction> QcomBroadcastActionPtr;
}



#endif




