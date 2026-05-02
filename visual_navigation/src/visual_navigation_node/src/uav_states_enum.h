#pragma once

#include <stdint.h>
#include <string_view>
#include "StateSystem/state_enum.h"
#include <type_traits>

enum class UAV_STATE: StateSystem::state_underlying_type{
    PREFLIGHT = static_cast<StateSystem::state_underlying_type>(StateSystem::STATE::STATE_0),
    INITIALIZE_CAMERA,
    INITIALIZE_RANGEFINDER,
    INITIALIZE_IMU,
    INITIALIZE_FLIGHT_MANAGER,
    TAKEOFF,
    INITIALIZE_NAVIGATION_MANAGER,
    WORKING
};

inline std::string_view get_uav_state_string_presentation(UAV_STATE event_type){
        switch (event_type)
        {
        case UAV_STATE::PREFLIGHT:
            return "PREFLIGHT STATE";

        case UAV_STATE::INITIALIZE_CAMERA:
            return "CAMERA INITIALIZATION STATE";

        case UAV_STATE::INITIALIZE_RANGEFINDER:
            return "RANGEFINDER INITIALIZATION STATE";

        case UAV_STATE::INITIALIZE_IMU:
            return "IMU INITIALIZATION STATE";

        case UAV_STATE::INITIALIZE_FLIGHT_MANAGER:
            return "FLIGHT MANAGER INITIALIZATON STATE";
            
        case UAV_STATE::TAKEOFF:
            return "UAV TAKEOFF STATE";
            
        case UAV_STATE::INITIALIZE_NAVIGATION_MANAGER:
            return "NAVIGATION MANAGER INITIALIZATON STATE";
            
        case UAV_STATE::WORKING:
            return "UAV WORKING STATE";
    }
    return {};
}


inline StateSystem::STATE convert_uav_state_to_base_state(UAV_STATE state){
    return static_cast<StateSystem::STATE>(static_cast<StateSystem::state_underlying_type>(state));
}

inline UAV_STATE convert_base_state_to_uav_state(StateSystem::STATE state){
    return static_cast<UAV_STATE>(static_cast<StateSystem::state_underlying_type>(state));
}