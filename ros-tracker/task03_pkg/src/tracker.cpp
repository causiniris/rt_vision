#include <rclcpp/rclcpp.hpp>

#include "task03_pkg/msg/tracker_params.hpp"

class TrackerNode : public rclcpp::Node {
public:
    TrackerNode() : Node("tracker_node") {
        // 创建订阅者
        sub_ = this->create_subscription<task03_pkg::msg::TrackerParams>(
            "tracker_config", 10,
            std::bind(&TrackerNode::topic_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Tracker 等待配置中...");
    }

private:
    void topic_callback(const task03_pkg::msg::TrackerParams::SharedPtr msg) {
        // 收到消息后的回调函数
        RCLCPP_INFO(this->get_logger(),
                    "Tracker 收到配置! xyz=%.3f, yaw=%.3f, r=%.3f | vx=%d, vy=%d, vz=%d",
                    msg->ekf_xyz, msg->ekf_yaw, msg->ekf_r, msg->vx, msg->vy, msg->vz);
    }

    rclcpp::Subscription<task03_pkg::msg::TrackerParams>::SharedPtr sub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TrackerNode>());
    rclcpp::shutdown();
    return 0;
}