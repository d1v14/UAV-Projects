#pragma once
#include "../UAVEvents/imu_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Imu.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2/utils.h>
#include <limits>
#include <optional>

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
        void imu_callback(const sensor_msgs::ImuConstPtr& msg);

    private:
        ros::Subscriber imu_subscriber{};
        std::optional<double> roll{std::nullopt};
        std::optional<double> pitch{std::nullopt};
        std::optional<double> yaw{std::nullopt};
    };
}