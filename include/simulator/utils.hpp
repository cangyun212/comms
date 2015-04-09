#ifndef __SIMULATOR_UTILS_HPP__
#define __SIMULATOR_UTILS_HPP__

#include "core/core.hpp"
#include "core/core_utils.hpp"

#include "simulator/predeclare.hpp"


#include "simulator/action_center.hpp"
#include "simulator/action.hpp"
#include "simulator/action_factory.hpp"
#include "simulator/simulator.hpp"


namespace sg {

    SimulatorPtr setup_sim();
    ActionFactoryPtr setup_action_factory();
    bool quit_sim();

}


#endif



