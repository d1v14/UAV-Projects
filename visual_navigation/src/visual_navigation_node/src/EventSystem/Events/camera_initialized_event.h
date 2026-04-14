#include "base_event.h"

class CameraInitializedEvent: public BaseEvent{
public:
    CameraInitializedEvent(float focal_range):BaseEvent(EventType::CAMERA_INITIALIZED), focal_range(focal_range){

    }

private:
    float focal_range{};    
};