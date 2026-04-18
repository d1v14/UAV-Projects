#pragma once
#include "../../../EventSystem/Events/base_event.h"
#include "flight_manager_events_enum.h"

class TakeoffDoneEvent: public EventSystem::BaseEvent{
public:
    TakeoffDoneEvent():BaseEvent(convert_flight_manager_event_type_to_base_event_type(FLIGHT_MANAGER_EVENT::TAKEOFF_DONE)){

    }
};
