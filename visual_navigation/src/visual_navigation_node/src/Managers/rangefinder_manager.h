#pragma once
#include "../UAVEvents/rangefinder_initialized_event.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Range.h>
#include <limits>

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
        void rangefinder_callback(const sensor_msgs::RangeConstPtr& msg);

    private:
        ros::Subscriber rangifinder_subscriber{};
        std::optional<float> max_range{std::nullopt};
        std::optional<float> min_range{std::nullopt};
        std::optional<float> range{std::nullopt};

    };
}