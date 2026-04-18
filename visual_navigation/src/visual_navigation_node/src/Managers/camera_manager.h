#pragma once
#include "../UAVEvents/camera_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/CameraInfo.h>
#include <ros/ros.h>
#include <optional>
#include <limits>

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
        void camera_info_callback(const sensor_msgs::CameraInfoConstPtr& msg);

    private:
        ros::Subscriber camera_subscriber{};
        std::optional<double> focus_range{std::nullopt};
    };
}