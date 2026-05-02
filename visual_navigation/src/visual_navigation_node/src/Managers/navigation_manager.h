#pragma once
#include "../UAVEvents/camera_initialized_event.h"
#include <ros/ros.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/Range.h>
#include <optional>
#include <limits>
#include "../UAVEvents/navigation_manager_initialized_event.h"

namespace Managers{
    class NavigationManager{
    private:
        using MySyncPolicy = message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Imu, sensor_msgs::Range>;

    public:
        NavigationManager(){
            initialize();
        }
        ~NavigationManager() = default;

    public:

    private:
        void initialize();
        void synchronized_data_callback(const sensor_msgs::ImageConstPtr& img, const sensor_msgs::ImuConstPtr& imu, const sensor_msgs::RangeConstPtr& rng);

    private:
        std::optional<message_filters::Subscriber<sensor_msgs::Image>> image_subscriber;
        std::optional<message_filters::Subscriber<sensor_msgs::Imu>> imu_subscriber;
        std::optional<message_filters::Subscriber<sensor_msgs::Range>> range_subscriber;

        std::unique_ptr<message_filters::Synchronizer<MySyncPolicy>> syncronizer;
    };
}