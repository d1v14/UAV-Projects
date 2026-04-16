#pragma once

#include <stdint.h>
#include <string_view>

enum class UAV_STATE: uint8_t{
    PREFLIGHT = 0,
    INITIALIZE_CAMERA,
    INITIALIZE_RANGEFINDER,
    INITIALIZE_IMU,
    ARMING,
    TAKEOFF,
    READY
};

std::string_view get_uav_state_name(UAV_STATE event_type){
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
            
        case UAV_STATE::ARMING:
            return "UAV ARMING STATE";
            
        case UAV_STATE::TAKEOFF:
            return "UAV TAKEOFF STATE";
            
        case UAV_STATE::READY:
            return "UAV READY STATE";
    }
}