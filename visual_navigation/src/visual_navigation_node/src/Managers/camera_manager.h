#pragma once
#include "../EventSystem/Events/camera_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/CameraInfo.h>
#include <ros/ros.h>

namespace Managers{
    class CameraManager{
    public:
        CameraManager(){
            initialize();
        }
        ~CameraManager() = default;

    public:

    private:
        void initialize();

    private:
        ros::Subscriber camera_subscriber{}
    };
}