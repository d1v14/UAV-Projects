#pragma once

#include <stdint.h>
#include <string_view>
#include <limits>

namespace EventSystem 
{
    using event_type_underlying_type = uint16_t;
    
    enum class EventType: event_type_underlying_type{
        EVENT_TYPE_0,
        EVETN_TYPE_MAX = std::numeric_limits<event_type_underlying_type>::max()
    };

    // std::string_view get_event_name(EventType event_type){
    //     switch (event_type)
    //     {
    //     case EventType::START:
    //         return "START EVENT";

    //     case EventType::CAMERA_INITIALIZED:
    //         return "UAV CAMERA INITIALIZATION END";
            
    //     case EventType::IMU_INITIALIZED:
    //         return "UAV IMU INITIALIZATION END";
            
    //     case EventType::UAV_ARMED:
    //         return "UAV SUCCESSFULLY ARMED";
            
    //     case EventType::UAV_READY:
    //         return "UAV SUCCESSFULLY TAKEOFF";    
    //     }
    // }


    class BaseEvent{
    public:
        BaseEvent(EventType event_type):event_type(event_type){};
        virtual ~BaseEvent() = default;
    public:
        EventType get_event_type() const {
            return this->event_type;    
        }
    private:
        EventType event_type{EventType::EVENT_TYPE_0};
    };
}