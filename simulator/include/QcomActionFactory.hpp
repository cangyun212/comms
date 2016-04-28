#ifndef __SG_QCOM_ACTION_FACTORY_HPP__
#define __SG_QCOM_ACTION_FACTORY_HPP__

#include "Core.hpp"

#include "Predeclare.hpp"
#include "ActionFactory.hpp"

namespace sg 
{

    class QcomActionFactory : public ActionFactory
    {
    public:
        QcomActionFactory();
        ~QcomActionFactory();

    public:
        void        Init() override;
        
    private:
    };

}


#endif


