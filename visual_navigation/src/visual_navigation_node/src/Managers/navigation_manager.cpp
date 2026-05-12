#include "navigation_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::NavigationManager::initialize()
{
    good_points.clear();
    image_subscriber.emplace();
    imu_subscriber.emplace();
    range_subscriber.emplace();
    if(image_subscriber && imu_subscriber && range_subscriber) 
    {
        auto node_handle = VisualNavigationNodeWorkflow::node_handle();
        image_subscriber->subscribe(node_handle, "/iris_visual_navigation/camera1/image_raw", 1);
        imu_subscriber->subscribe(node_handle, "/mavros/imu/data", 10);
        range_subscriber->subscribe(node_handle, "/range_down", 5);
        vision_speed_publisher = node_handle.advertise<geometry_msgs::TwistStamped>("mavros/vision_speed/speed_twist", 10); 

        syncronizer = std::make_unique<message_filters::Synchronizer<MySyncPolicy>>(
            MySyncPolicy(100), *image_subscriber, *imu_subscriber, *range_subscriber
        );

        this->thread_pool.start();
        good_points.clear();
        
        syncronizer->registerCallback(boost::bind(&NavigationManager::synchronized_data_callback, this, _1, _2, _3));
        VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::NavigationManagerInitializedEvent>());
    }
}

void Managers::NavigationManager::synchronized_data_callback(const sensor_msgs::ImageConstPtr &img, const sensor_msgs::ImuConstPtr &imu, const sensor_msgs::RangeConstPtr &rng)
{
    static std::future<std::vector<cv::Point2f>> good_point_generating_done{};
    cv::Mat frame = cv_bridge::toCvShare(img, "mono8")->image;
    if(previous_frame.empty() || !is_pipeline_enabled)
    {
        previous_frame = frame;
        previous_time = img->header.stamp;
        return;
    }

    if(good_point_generating_done.valid())
    {
        if(good_point_generating_done.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            std::vector<cv::Point2f> new_pts = good_point_generating_done.get();
            good_points.insert(good_points.end(), new_pts.begin(), new_pts.end());
            ROS_INFO("Merged %zu new points. Total: %zu", new_pts.size(), good_points.size());
        }
    }

    if(good_points.empty())
    {
        good_point_generating_done  = thread_pool.add_task([this, frame_clone = frame.clone()](){ return append_good_points_task(frame_clone, this->max_good_points);});
        return;
    }

        
    double dt = (img->header.stamp - previous_time).toSec();
    auto imu_predicted_points = predict_points_by_imu(this->good_points,imu,dt);

    std::tie(good_points,imu_predicted_points) = calculate_lucas_kanade(previous_frame, frame, good_points, imu_predicted_points);
    std::tie(good_points,imu_predicted_points) = ransac_filter(good_points, imu_predicted_points);
    calculate_velocity(imu,good_points,imu_predicted_points,dt,rng->range);

    previous_frame = frame.clone();
    previous_time = img->header.stamp;
    good_points = std::move(imu_predicted_points);

    if(good_points.size() < 85 && !good_point_generating_done.valid())
    {
        good_point_generating_done  = thread_pool.add_task([this, frame_clone = frame.clone()](){ return append_good_points_task(frame_clone, this->max_good_points - good_points.size());});
    }
}

bool Managers::NavigationManager::clahe_preprocessing(cv::Mat& image){
    static thread_local auto clahe = cv::createCLAHE(3., cv::Size(8,8));
    clahe->apply(image,image);
    return true;
}

std::vector<cv::Point2f> Managers::NavigationManager::detect_new_features(const cv::Mat& img, std::size_t count){
    static thread_local std::size_t points_count = max_good_points;
    static thread_local std::vector<cv::Point2f> new_points(points_count);
    new_points.clear();
    cv::goodFeaturesToTrack(img, new_points, count, 0.01, 20);
    ROS_INFO("FOUND %d POINTS", new_points.size());
    return new_points;
}

std::pair<std::vector<cv::Point2f> &, std::vector<cv::Point2f> &> Managers::NavigationManager::calculate_lucas_kanade(const cv::Mat &previous_frame, const cv::Mat &current_frame, std::vector<cv::Point2f> &good_points, std::vector<cv::Point2f> &predicted_points)
{
    static std::vector<uchar> points_status{max_good_points};
    static std::vector<float> points_error{max_good_points};

    points_status.clear();
    points_error.clear();

    cv::calcOpticalFlowPyrLK(
    previous_frame,     
    current_frame,      
    good_points,
    predicted_points,  
    points_status,          
    points_error,       
    cv::Size(21,21), 
    3,               
    cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 30, 0.01),
    cv::OPTFLOW_USE_INITIAL_FLOW 
    );

    ROS_INFO("POINTS SIZE BEFORE LUCAS %d", predicted_points.size());
    good_points = filter_points(good_points,points_status);
    predicted_points = filter_points(predicted_points,points_status);
    ROS_INFO("POINTS SIZE AFTER LUCAS %d", predicted_points.size());
    return {good_points, predicted_points};
}

std::vector<cv::Point2f> Managers::NavigationManager::predict_points_by_imu(const std::vector<cv::Point2f>& good_points, const sensor_msgs::ImuConstPtr& imu, double dt) 
{
    std::vector<cv::Point2f> predicted_pts;
    const double f = 476.7;
    float du = f * imu->angular_velocity.y * dt;
    float dv = -f * imu->angular_velocity.x * dt;

    predicted_pts.clear();
    for (const auto& p : good_points)
        predicted_pts.push_back(cv::Point2f(p.x + du, p.y + dv));
    
    return predicted_pts;
}

std::vector<cv::Point2f>& Managers::NavigationManager::filter_points(std::vector<cv::Point2f>& points, const std::vector<uchar>& mask) 
{
    auto points_write_it = points.begin();
    auto points_read_it = points.begin();
    auto mask_it = mask.begin();

    for (; points_read_it != points.end(); points_read_it++, mask_it++) {
        if (*mask_it) { 
            if (points_read_it != points_write_it) {
                *points_write_it = std::move(*points_read_it);
            }
            ++points_write_it;
        }
    }
    points.erase(points_write_it, points.end());
    return points;
}

std::pair<std::vector<cv::Point2f>&, std::vector<cv::Point2f>&> Managers::NavigationManager::ransac_filter(std::vector<cv::Point2f>& good_points, std::vector<cv::Point2f>& predicted_points) 
{
    if (predicted_points.size() < 4) {
        good_points.clear();
        predicted_points.clear();
        return {good_points, predicted_points};
    }

    static std::vector<uchar> ransac_mask;
    ransac_mask.clear();

    cv::findHomography(good_points, predicted_points, cv::RANSAC, 3.0, ransac_mask);

    ROS_INFO("POINTS SIZE BEFORE RANSAC %d", predicted_points.size());
    filter_points(good_points, ransac_mask);
    filter_points(predicted_points, ransac_mask);
    ROS_INFO("POINTS SIZE AFTER RANSAC %d", predicted_points.size());

    return {good_points, predicted_points};
}

void Managers::NavigationManager::calculate_velocity(const sensor_msgs::ImuConstPtr& imu,const std::vector<cv::Point2f>& good_points, const std::vector<cv::Point2f>& predicted_points, double dt, double altitude) 
{
    if (good_points.size() < 25 || dt <= 0) {
        return; 
    }

    float sum_du = 0;
    float sum_dv = 0;
    size_t n = good_points.size();

    for (auto predicted_point = predicted_points.begin(), good_point = good_points.begin();
        predicted_point != predicted_points.end() && good_point != good_points.end(); good_point++, predicted_point++) 
    {
        sum_du += (predicted_point->x - good_point->x);
        sum_dv += (predicted_point->y - good_point->y);
    }
    float avg_du = sum_du / n; 
    float avg_dv = sum_dv / n; 

    const float f = 476.7f;
    float du_imu = -f * (float)imu->angular_velocity.x * dt; 
    float dv_imu = f * (float)imu->angular_velocity.y * dt;

    float pure_trans_du = avg_du + du_imu;
    float pure_trans_dv = avg_dv + dv_imu;

    float vx_raw = (pure_trans_dv / f) * (altitude / dt);
    float vy_raw = (pure_trans_du / f) * (altitude / dt);


    if (std::abs(vx_raw) < 0.07) vx_raw = 0.0f;
    if (std::abs(vy_raw) < 0.07) vy_raw = 0.0f;


    static float filtered_vx = 0, filtered_vy = 0;
    float alpha = 0.15f; 
    filtered_vx = alpha * vx_raw + (1.0f - alpha) * filtered_vx;
    filtered_vy = alpha * vy_raw + (1.0f - alpha) * filtered_vy;

    geometry_msgs::TwistStamped visual_msg;
    visual_msg.header.stamp = ros::Time::now();
    visual_msg.header.frame_id = "base_link"; 
    visual_msg.twist.linear.x = filtered_vx; 
    visual_msg.twist.linear.y = filtered_vy;
    visual_msg.twist.linear.z = 0.0;

    vision_speed_publisher->publish(visual_msg);

    ROS_INFO("Скорость: VX: %.2f, VY: %.2f | H: %.2f", filtered_vx, filtered_vy, altitude);
    ROS_INFO("DU: %.2f | IMU_U: %.2f | DIFF: %.2f", avg_du, du_imu, pure_trans_du);
    ROS_INFO("DV: %.2f | IMU_V: %.2f | DIFF: %.2f", avg_dv, dv_imu, pure_trans_dv);
}

std::vector<cv::Point2f> Managers::NavigationManager::append_good_points_task(cv::Mat img, std::size_t count)
{
    clahe_preprocessing(img);
    return detect_new_features(img,count);
}
