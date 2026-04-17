#pragma once
#include "../EventSystem/Events/uav_armed_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>

namespace Managers{
    class FlightManager{
    public:
        FlightManager(){
            initialize();
        }
        ~FlightManager() = default;

    public:
        void arm();
        void takeoff();


    private:
        void initialize();

    private:
    };
}