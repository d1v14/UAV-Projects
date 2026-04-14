#include <stdint.h>

namespace EventSystem 
{
    enum class EventType: uint16_t{
    UNKNOWN = 0,
    PREFLIGHT,
    CAMERA_INITIALIZED,
    IMU_INITIALIZED,
    RANGEFINDER_INTIALIZED,
    UAV_ARMED,
    UAV_READY
    };


    class BaseEvent{
    public:
        BaseEvent(EventType event_type):event_type(event_type){};
        virtual ~BaseEvent() = default;
    public:
        EventType get_event_type() const {
            return this->event_type;    
        }
    private:
        EventType event_type{EventType::UNKNOWN};
    };
}