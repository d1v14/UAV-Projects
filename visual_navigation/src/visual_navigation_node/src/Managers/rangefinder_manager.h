#pragma once
#include "../EventSystem/Events/rangefinder_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>

namespace Managers{
    class RangefinderManager{
    public:
        RangefinderManager(){
            initialize();
        }
        ~RangefinderManager() = default;

    public:

    private:
        void initialize();

    private:
    };
}