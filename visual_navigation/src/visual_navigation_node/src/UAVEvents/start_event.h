#pragma once
#include "../EventSystem/Events/base_event.h"
#include "uav_event_enum.h"
namespace EventSystem{
    
class StartEvent: public BaseEvent{
public:
    StartEvent():BaseEvent(convert_uav_event_type_to_base_event_type(UAV_EVENT::START)){

    }

private:
};

};