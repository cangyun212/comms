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
    class QcomGameConfigurationChangeAction;
    class QcomPurgeEventsAction;
    class QcomEGMParametersAction;
    class QcomProgressiveConfigAction;
    class QcomTimeDateAction;
    class QcomLPCurrentAmountAction;
    class QcomGeneralPromotionalAction;
    class QcomSiteDetailAction;
    class QcomEGMGeneralStatusAction;
    class QcomPendingAction;
    class QcomSendAction;
    class ActionFactory;
    class QcomActionFactory;
    class Simulator;
    class QcomSim;
    
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
    typedef std::shared_ptr<QcomGameConfigurationChangeAction> QcomGameConfigurationChangeActionPtr;
    typedef std::shared_ptr<QcomEGMParametersAction> QcomEGMParametersActionPtr;
    typedef std::shared_ptr<QcomProgressiveConfigAction> QcomProgressiveConfigActionPtr;
    typedef std::shared_ptr<QcomPurgeEventsAction> QcomPurgeEventsActionPtr;
    typedef std::shared_ptr<QcomTimeDateAction> QcomTimeDateActionPtr;
    typedef std::shared_ptr<QcomLPCurrentAmountAction> QcomLPCurrentAmountActionPtr;
    typedef std::shared_ptr<QcomGeneralPromotionalAction> QcomGeneralPromotionalActionPtr;
    typedef std::shared_ptr<QcomSiteDetailAction> QcomSiteDetailActionPtr;
    typedef std::shared_ptr<QcomEGMGeneralStatusAction> QcomEGMGeneralStatusActionPtr;
    typedef std::shared_ptr<QcomPendingAction> QcomPendingActionPtr;
    typedef std::shared_ptr<QcomSendAction> QcomSendActionPtr;
}



#endif




