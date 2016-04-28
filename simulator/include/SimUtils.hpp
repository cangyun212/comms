#ifndef __SG_SIMULATOR_UTILS_HPP__
#define __SG_SIMULATOR_UTILS_HPP__

#include "Core.hpp"
#include "Utils.hpp"

#include "Predeclare.hpp"

#include "ActionCenter.hpp"
#include "Action.hpp"
#include "ActionFactory.hpp"
#include "Simulator.hpp"


namespace sg 
{

    SimulatorPtr setup_sim();
    ActionFactoryPtr setup_action_factory();
    bool quit_sim();

}


#endif



