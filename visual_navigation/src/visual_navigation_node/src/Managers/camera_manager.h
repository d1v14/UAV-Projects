#pragma once
#include "../EventSystem/Events/camera_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>

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
    };
}