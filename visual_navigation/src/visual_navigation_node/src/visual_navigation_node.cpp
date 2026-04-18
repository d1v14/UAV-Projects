#include <ros/ros.h>
#include <std_msgs/String.h>
#include "visual_navigation_node_worklow.h"

int main(int argc, char *argv[]){
    ros::init(argc, argv, "visual_navigation_node");
    VisualNavigationNodeWorkflow node_workflow{};
    node_workflow.start();
    ROS_INFO("Navigation node started!");
    ros::spin();
    return 0;
}