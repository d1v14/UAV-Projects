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

        syncronizer->registerCallback(boost::bind(&NavigationManager::synchronized_data_callback, this, _1, _2, _3));
        VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::NavigationManagerInitializedEvent>());
    }
}

void Managers::NavigationManager::synchronized_data_callback(const sensor_msgs::ImageConstPtr &img, const sensor_msgs::ImuConstPtr &imu, const sensor_msgs::RangeConstPtr &rng)
{
    ROS_INFO("GOT DATA");
}
