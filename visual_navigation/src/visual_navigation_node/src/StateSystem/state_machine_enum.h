#include <stdint.h>

enum class UAV_STATE: uint8_t{
    START = 0,
    INITIALIZE_CAMERA,
    INITIALIZE_RANGEFINDER,
    INITIALIZE_IMU,
    PREFLIGHT_PREPARED,
    ARMING,
    ARMED,
    TAKEOFF,
    READY
};