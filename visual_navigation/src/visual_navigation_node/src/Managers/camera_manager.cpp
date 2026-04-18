#include "camera_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::CameraManager::initialize()
{
    this->camera_subscriber = VisualNavigationNodeWorkflow::node_handle().subscribe("/iris_visual_navigation/camera1/camera_info", 1, &Managers::CameraManager::camera_info_callback, this);
}

void Managers::CameraManager::camera_info_callback(const sensor_msgs::CameraInfoConstPtr &msg)
{
    if(abs(msg->K[0] - 0) < std::numeric_limits<double>::epsilon())
        return ;

    this->focus_range = msg->K[0];
    ROS_INFO("[CAMERA MANAGER] Got focal range: %f",*this->focus_range);
    this->camera_subscriber.shutdown();
         VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::CameraInitializedEvent>(1));
}
