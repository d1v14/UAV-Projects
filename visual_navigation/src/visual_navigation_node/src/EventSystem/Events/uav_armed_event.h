#include "base_event.h"

namespace EventSystem{

class UAVArmedEvent: public BaseEvent{
public:
    UAVArmedEvent():BaseEvent(EventType::UAV_ARMED){

    }
};

}