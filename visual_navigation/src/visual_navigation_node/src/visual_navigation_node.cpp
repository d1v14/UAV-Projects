#include <ros/ros.h>
#include <std_msgs/String.h>

int main(int argc, char *argv[]){
    ros::init(argc, argv, "visual_navigation_node");
    ros::NodeHandle node_handle{};
    ROS_INFO("Navigation node started!");
    ros::spin();
    return 0;
}