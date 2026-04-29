#pragma once

#include <stdint.h>
#include <string_view>
#include "../../../StateSystem/state_enum.h"
#include <type_traits>

enum class FLIGHT_MANAGER_STATE: StateSystem::state_underlying_type{
    CREATED = static_cast<StateSystem::state_underlying_type>(StateSystem::STATE::STATE_0),
    START_POSITION_SENDER,
    DISABLING_RC,
    ENABLING_OFFBOARD,
    ARMING,
    TAKEOFF,
    READY
};

inline std::string_view get_flight_manager_state_string_presentation(FLIGHT_MANAGER_STATE event_type){
        switch (event_type)
        {
        case FLIGHT_MANAGER_STATE::CREATED:
            return "FLIGHT MANAGER CREATED STATE";

        case FLIGHT_MANAGER_STATE::DISABLING_RC:
            return "UAV DISABLING RC STATE";

        case FLIGHT_MANAGER_STATE::START_POSITION_SENDER:
            return "UAV STARTING POSITION SENDER";

        case FLIGHT_MANAGER_STATE::ENABLING_OFFBOARD:
            return "UAV ENABLING OFFBOARD STATE";

        case FLIGHT_MANAGER_STATE::ARMING:
            return "UAV ARMING STATE";

        case FLIGHT_MANAGER_STATE::TAKEOFF:
            return "UAV TAKEOFF STATE";
            
        case FLIGHT_MANAGER_STATE::READY:
            return "UAV READY STATE";
    }
    return {};
}


inline StateSystem::STATE convert_flight_manager_state_to_base_state(FLIGHT_MANAGER_STATE state){
    return static_cast<StateSystem::STATE>(static_cast<StateSystem::state_underlying_type>(state));
}

inline FLIGHT_MANAGER_STATE convert_base_state_to_flight_manager_state(StateSystem::STATE state){
    return static_cast<FLIGHT_MANAGER_STATE>(static_cast<StateSystem::state_underlying_type>(state));
}