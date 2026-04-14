#include "base_event.h"

class IMUInitializedEvent: public BaseEvent{
public:
    IMUInitializedEvent():BaseEvent(EventType::IMU_INITIALIZED){

    }
};
