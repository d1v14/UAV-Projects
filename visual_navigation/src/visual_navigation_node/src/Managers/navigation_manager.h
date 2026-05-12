#pragma once
#include "../UAVEvents/camera_initialized_event.h"
#include <ros/ros.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/Range.h>
#include <geometry_msgs/TwistStamped.h>
#include <optional>
#include <limits>
#include "../UAVEvents/navigation_manager_initialized_event.h"
#include "../ThreadPool/thread_pool.h"
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

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
        void enable_navidation(){
            this->is_pipeline_enabled = true;
        }

        void disable_navigation(){
            this->is_pipeline_enabled = false;
        }

    private:
        void initialize();
        void synchronized_data_callback(const sensor_msgs::ImageConstPtr& img, const sensor_msgs::ImuConstPtr& imu, const sensor_msgs::RangeConstPtr& rng);

    private:
        bool clahe_preprocessing(cv::Mat& image);
        std::vector<cv::Point2f> detect_new_features(const cv::Mat& img, std::size_t count);
        std::pair<std::vector<cv::Point2f> &, std::vector<cv::Point2f> &> calculate_lucas_kanade(const cv::Mat &previous_frame, const cv::Mat &current_frame, std::vector<cv::Point2f> &good_points, std::vector<cv::Point2f> &predicted_points);
        std::pair<std::vector<cv::Point2f>&, std::vector<cv::Point2f>&> ransac_filter(std::vector<cv::Point2f>& good_points, std::vector<cv::Point2f>& predicted_points);
        std::vector<cv::Point2f> predict_points_by_imu(const std::vector<cv::Point2f>& good_points, const sensor_msgs::ImuConstPtr& imu, double dt);
        std::vector<cv::Point2f>& filter_points(std::vector<cv::Point2f>& points, const std::vector<uchar>& mask);
        void calculate_velocity(const sensor_msgs::ImuConstPtr& imu,const std::vector<cv::Point2f>& good_points, const std::vector<cv::Point2f>& predicted_points, double dt, double altitude);
        std::vector<cv::Point2f> append_good_points_task(cv::Mat img, std::size_t count);


    private:
        std::optional<message_filters::Subscriber<sensor_msgs::Image>> image_subscriber;
        std::optional<message_filters::Subscriber<sensor_msgs::Imu>> imu_subscriber;
        std::optional<message_filters::Subscriber<sensor_msgs::Range>> range_subscriber;
        std::unique_ptr<message_filters::Synchronizer<MySyncPolicy>> syncronizer;
        std::optional<ros::Publisher> vision_speed_publisher;

    private:
        ThreadPool<6> thread_pool{};
        std::atomic<bool> is_pipeline_enabled{true};

    private:
        static inline constexpr std::size_t max_good_points{150};

    private:
        std::vector<cv::Point2f> good_points{max_good_points};
        
        cv::Mat previous_frame{};    
        ros::Time previous_time{};
        
    };
}