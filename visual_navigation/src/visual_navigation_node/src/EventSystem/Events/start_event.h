#pragma once
#include "base_event.h"

namespace EventSystem{
    
class StartEvent: public BaseEvent{
public:
    StartEvent():BaseEvent(EventType::START){

    }

private:
};

};