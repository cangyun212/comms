#ifndef __QCOM_ACTION_FACTORY_HPP__
#define __QCOM_ACTION_FACTORY_HPP__

#include "core/core.hpp"

#include "simulator/predeclare.hpp"
#include "simulator/action_factory.hpp"

namespace sg {

    class QcomActionFactory : public ActionFactory
    {
    public:
        QcomActionFactory();
        ~QcomActionFactory();

    public:
        void        Init() CORE_OVERRIDE;
        
    private:
    };

}


#endif


