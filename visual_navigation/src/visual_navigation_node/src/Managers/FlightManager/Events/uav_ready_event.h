#pragma once
#include "../../../EventSystem/Events/base_event.h"
#include "flight_manager_events_enum.h"


class UAVReadyEvent: public EventSystem::BaseEvent{
public:
    UAVReadyEvent():BaseEvent(convert_flight_manager_event_type_to_base_event_type(FLIGHT_MANAGER_EVENT::READY)){

    }
};
