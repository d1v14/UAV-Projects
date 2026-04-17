#pragma once
#include "../EventSystem/Events/imu_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>

namespace Managers{
    class IMUManager{
    public:
        IMUManager(){
            initialize();
        }
        ~IMUManager() = default;

    public:

    private:
        void initialize();

    private:
    };
}