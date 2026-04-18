#include "imu_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::IMUManager::initialize()
{
    this->imu_subscriber = VisualNavigationNodeWorkflow::node_handle().subscribe("/mavros/imu/data", 1, &Managers::IMUManager::imu_callback, this);
}

void Managers::IMUManager::imu_callback(const sensor_msgs::ImuConstPtr &msg)
{
    bool state_before = this->roll && this->pitch && this->yaw;
    if(abs(msg->orientation.x - 0) < std::numeric_limits<double>::epsilon() &&
       abs(msg->orientation.y - 0) < std::numeric_limits<double>::epsilon() &&
       abs(msg->orientation.z - 0) < std::numeric_limits<double>::epsilon() &&
       abs(msg->orientation.w - 0) < std::numeric_limits<double>::epsilon())
        return;

    tf2::Quaternion quaternion(msg->orientation.x, msg->orientation.y,msg->orientation.z,msg->orientation.w);
    tf2::Matrix3x3 matrix(quaternion);
    double roll{};
    double pitch{};
    double yaw{};
    matrix.getRPY(roll,pitch,yaw);
    this->roll.emplace(roll);
    this->pitch.emplace(pitch);
    this->yaw.emplace(yaw);

    bool state_after = this->roll && this->pitch && this->yaw;
    if(state_before != state_after)
        state_after ? VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::IMUInitializedEvent>()) : void();
}
