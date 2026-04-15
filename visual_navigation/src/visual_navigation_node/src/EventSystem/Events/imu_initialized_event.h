#include "base_event.h"

namespace EventSystem{

class IMUInitializedEvent: public BaseEvent{
public:
    IMUInitializedEvent():BaseEvent(EventType::IMU_INITIALIZED){

    }
};

}
