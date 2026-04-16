#pragma once
#include "base_event.h"

namespace EventSystem{

class UAVReadyEvent: public BaseEvent{
public:
    UAVReadyEvent():BaseEvent(EventType::UAV_READY){

    }
};

}