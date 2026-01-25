#include <rclcpp/rclcpp.hpp>

#include "task03_pkg/msg/tracker_params.hpp"

using namespace std::chrono_literals;

class FounderNode : public rclcpp::Node {
public:
    FounderNode() : Node("founder_node") {
        // 1. 声明参数 (参数名, 默认值)
        this->declare_parameter("ekf_xyz", 0.0);
        this->declare_parameter("ekf_yaw", 0.0);
        this->declare_parameter("ekf_r", 0.0);
        this->declare_parameter("vx", 0);
        this->declare_parameter("vy", 0);
        this->declare_parameter("vz", 0);

        // 2. 读取参数 (从 YAML 加载)
        params_.ekf_xyz = this->get_parameter("ekf_xyz").as_double();
        params_.ekf_yaw = this->get_parameter("ekf_yaw").as_double();
        params_.ekf_r   = this->get_parameter("ekf_r").as_double();
        params_.vx      = this->get_parameter("vx").as_int();
        params_.vy      = this->get_parameter("vy").as_int();
        params_.vz      = this->get_parameter("vz").as_int();

        // 打印确认一下读取结果
        RCLCPP_INFO(this->get_logger(), "Founder 就绪! 读取到 ekf_xyz: %.3f, vx: %d",
                    params_.ekf_xyz, params_.vx);

        // 3. 创建发布者
        pub_ = this->create_publisher<task03_pkg::msg::TrackerParams>("tracker_config", 10);

        // 4. 设置定时器 (1秒发一次)
        timer_ = this->create_wall_timer(1000ms, std::bind(&FounderNode::timer_callback, this));
    }

private:
    void timer_callback() {
        pub_->publish(params_);
        RCLCPP_INFO(this->get_logger(), "已发送配置参数 -> Tracker");
    }

    rclcpp::Publisher<task03_pkg::msg::TrackerParams>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    task03_pkg::msg::TrackerParams params_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FounderNode>());
    rclcpp::shutdown();
    return 0;
}