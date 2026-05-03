#include "navigation_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::NavigationManager::initialize()
{
    image_subscriber.emplace();
    imu_subscriber.emplace();
    range_subscriber.emplace();
    if(image_subscriber && imu_subscriber && range_subscriber) 
    {
        auto node_handle = VisualNavigationNodeWorkflow::node_handle();
        image_subscriber->subscribe(node_handle, "/iris_visual_navigation/camera1/image_raw", 1);
        imu_subscriber->subscribe(node_handle, "/mavros/imu/data", 10);
        range_subscriber->subscribe(node_handle, "/range_down", 5);

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
    cv::Mat frame = cv_bridge::toCvShare(img, "mono8")->image;
    auto clahe_future = thread_pool.add_task([this, &frame](){ clahe_preprocessing(frame);});
    clahe_future.get();
    ROS_INFO("CLAHE PROCESSED");

    ROS_INFO("GOOD POINTS SIZE %d", good_points.size());
    auto points_future  = thread_pool.add_task([this, &frame](){ detect_new_features(frame, good_points);});
    points_future.get();
    
    for (const auto& point : good_points) {
        // Рисуем закрашенный круг
        cv::circle(frame, 
                   point,           // Центр точки
                   3,               // Радиус
                   cv::Scalar(0, 255, 0), // Цвет (B, G, R) - зеленый
                   -1               // Толщина (-1 значит закрашенный)
        );
    }

    cv::imshow("Optical Flow Debug", frame);
    cv::waitKey(1); 
}

bool Managers::NavigationManager::clahe_preprocessing(cv::Mat& image){
    static thread_local auto clahe = cv::createCLAHE(3., cv::Size(8,8));
    clahe->apply(image,image);
    return true;
}

bool Managers::NavigationManager::detect_new_features(const cv::Mat& img, std::vector<cv::Point2f>& pts){
    static thread_local std::size_t points_count = max_good_points;
    static thread_local std::vector<cv::Point2f> new_points(points_count);
    new_points.clear();
    ROS_INFO("NEW POINTS SIZE BEFORE %d", new_points.size());
    if(pts.size() <  max_good_points)
    {
        cv::goodFeaturesToTrack(img, new_points, points_count - pts.size(), 0.01, 20);
        ROS_INFO("NEW POINTS SIZE AFTER %d", new_points.size());
        pts.insert(pts.end(), new_points.begin(), new_points.end());
    }
    return true;
}
