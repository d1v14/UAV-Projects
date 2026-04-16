#include <ros/ros.h>
#include <std_msgs/String.h>
#include "visual_navigation_node_worklow.h"
#include "EventSystem/Events/start_event.h"

int main(int argc, char *argv[]){
    ros::init(argc, argv, "visual_navigation_node");
    VisualNavigationNodeWorkflow node_workflow{};
    ROS_INFO("Navigation node started!");
    node_workflow.event_queue().push_event(std::make_unique<EventSystem::StartEvent>());
    node_workflow.event_queue().start();
    
    ros::spin();
    return 0;
}