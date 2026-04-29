#pragma once
#include "../../../EventSystem/Events/base_event.h"

enum class FLIGHT_MANAGER_EVENT: EventSystem::event_type_underlying_type{
    FLIGHT_MANAGER_CREATED = static_cast<EventSystem::event_type_underlying_type>(EventSystem::EventType::EVENT_TYPE_0),
    POSITION_SENDER_CREATED,
    RC_DISABLED,
    OFFBOARD_ENABLED,
    ARMED,
    TAKEOFF_DONE,
    READY
};


inline EventSystem::EventType convert_flight_manager_event_type_to_base_event_type(FLIGHT_MANAGER_EVENT event){
    return static_cast<EventSystem::EventType>(static_cast<EventSystem::event_type_underlying_type>(event));
}

inline FLIGHT_MANAGER_EVENT convert_base_event_type_to_flight_manager_event_type(EventSystem::EventType event){
    return static_cast<FLIGHT_MANAGER_EVENT>(static_cast<EventSystem::event_type_underlying_type>(event));
}