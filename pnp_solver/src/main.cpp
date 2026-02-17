#include <rclcpp/rclcpp.hpp>
#include "pnp_solver/pnp_solver.hpp"

// 使用 C++17 的 string_view 避免不必要的字符串拷贝
using namespace std::string_literals;

class PnPDemoNode : public rclcpp::Node {
public:
    PnPDemoNode() : Node("pnp_demo_node") {
        RCLCPP_INFO(this->get_logger(), "正在初始化 PNP 解算节点...");
        
        try {
            run_homework_solution();
        } catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "运行时发生未捕获异常: %s", e.what());
        }
    }

private:
    void run_homework_solution() {
        // --- 1. 准备数据  ---
        
        // K = [fx, 0, cx; 0, fy, cy; 0, 0, 1]
        cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 
            1462.3697, 0, 398.59394,
            0, 1469.68385, 110.68997,
            0, 0, 1);

        // 畸变系数 (k1, k2, p1, p2, k3)
        cv::Mat dist_coeffs = (cv::Mat_<double>(1, 5) << 
            0.003518, -0.311778, -0.016581, 0.023682, 0.0000);

        // 实例化解算器
        auto solver = std::make_unique<robotac_vision::PnPSolver>(camera_matrix, dist_coeffs);

        // --- 2. 定义坐标点 ---
        
        // 图像坐标 (2D, Pixel) - 题目给定
        // A(100, 150), B(300, 150), C(300, 300), D(100, 300)
        std::vector<cv::Point2f> image_points = {
            {100.0f, 150.0f}, // A
            {300.0f, 150.0f}, // B
            {300.0f, 300.0f}, // C
            {100.0f, 300.0f}  // D
        };

        // 世界坐标 (3D, cm) 
        // 注意：PNP解算出的 tvec 单位将与这里保持一致 (即 cm)
        // A(0,0,0), B(10,0,0), C(10,10,0), D(0,10,0)
        std::vector<cv::Point3f> object_points = {
            {0.0f,  0.0f,  0.0f}, // A
            {10.0f, 0.0f,  0.0f}, // B
            {10.0f, 10.0f, 0.0f}, // C
            {0.0f,  10.0f, 0.0f}  // D
        };

        RCLCPP_INFO(this->get_logger(), "数据准备完毕，开始解算...");

        // --- 3. 执行解算 ---
        auto result = solver->solve(object_points, image_points, cv::SOLVEPNP_ITERATIVE);

        // --- 4. 输出结果 (核心代码归纳) ---
        if (result.success) {
            std::stringstream ss;
            ss << "\n================ PNP 解算结果 ================\n";
            ss << "1. 平移向量 (Translation Vector) [cm]:\n" << result.tvec << "\n";
            ss << "   -> 说明: 相机坐标系原点到物体坐标系原点的位移。\n";
            
            ss << "2. 旋转矩阵 (Rotation Matrix):\n" << result.rotation_matrix << "\n";
            ss << "   -> 说明: 描述物体相对于相机的姿态。\n";
            
            ss << "3. 目标距离 (Distance): " << result.distance << " cm\n";
            ss << "   -> 原理: sqrt(x^2 + y^2 + z^2)\n";
            ss << "==============================================";
            
            // 使用 ROS 日志输出
            RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(), "PNP 解算失败！请检查点位对应关系。");
        }
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PnPDemoNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}