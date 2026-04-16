#pragma once
#include "base_event.h"


namespace EventSystem{


class RangeFinderIntitalizedEvent: public BaseEvent{
public:
    RangeFinderIntitalizedEvent():BaseEvent(EventType::RANGEFINDER_INTIALIZED){

    }
};

}