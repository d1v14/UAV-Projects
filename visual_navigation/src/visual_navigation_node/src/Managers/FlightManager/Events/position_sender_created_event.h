#pragma once
#include "../../../EventSystem/Events/base_event.h"
#include "flight_manager_events_enum.h"

class PositionSenderCreatedEvent: public EventSystem::BaseEvent{
public:
    PositionSenderCreatedEvent():BaseEvent(convert_flight_manager_event_type_to_base_event_type(FLIGHT_MANAGER_EVENT::POSITION_SENDER_CREATED)){

    }
};
