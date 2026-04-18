#pragma once
#include "../EventSystem/Events/base_event.h"

enum class UAV_EVENT: EventSystem::event_type_underlying_type{
    UNKNOWN = static_cast<EventSystem::event_type_underlying_type>(EventSystem::EventType::EVENT_TYPE_0),
    START,
    CAMERA_INITIALIZED,
    IMU_INITIALIZED,
    RANGEFINDER_INTIALIZED,
    UAV_READY
};

inline EventSystem::EventType convert_uav_event_type_to_base_event_type(UAV_EVENT event){
    return static_cast<EventSystem::EventType>(static_cast<EventSystem::event_type_underlying_type>(event));
}

inline UAV_EVENT convert_base_event_type_to_uav_event_type(EventSystem::EventType event){
    return static_cast<UAV_EVENT>(static_cast<EventSystem::event_type_underlying_type>(event));
}