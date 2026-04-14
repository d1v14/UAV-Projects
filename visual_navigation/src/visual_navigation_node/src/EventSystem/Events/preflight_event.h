#include "base_event.h"

class PreflightEvent: public BaseEvent{
    public:
    PreflightEvent():BaseEvent(EventType::PREFLIGHT){

    }
};