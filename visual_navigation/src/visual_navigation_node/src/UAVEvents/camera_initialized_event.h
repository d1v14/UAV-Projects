#pragma once
#include "../EventSystem/Events/base_event.h"
#include "uav_event_enum.h"

namespace EventSystem{
    
class CameraInitializedEvent: public BaseEvent{
public:
    CameraInitializedEvent(float focal_range):BaseEvent(convert_uav_event_type_to_base_event_type(UAV_EVENT::CAMERA_INITIALIZED)), focal_range(focal_range){

    }

private:
    float focal_range{};    
};

};
