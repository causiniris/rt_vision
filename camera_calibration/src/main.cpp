#include "camera_calibration/camera_calibrator.hpp"
#include <filesystem>

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    // 请根据实际情况修改标定板参数
    cv::Size board_size(9, 6);
    float square_size = 0.025f;

    auto calibrator = std::make_unique<robotac_vision::CameraCalibrator>(board_size, square_size);

    // 鲁棒的路径查找：寻找 image 文件夹
    std::string img_path;
    std::filesystem::path current_path = std::filesystem::current_path();
    // 1. 检查当前路径下是否有 image (针对 ros2 run 或在包根目录运行)
    if (std::filesystem::exists(current_path / "image")) {
        img_path = (current_path / "image").string();
    } 
    // 2. 检查 share 目录 (针对标准安装后的路径)
    else if (std::filesystem::exists(current_path / "share" / "camera_calibration" / "image")) {
        img_path = (current_path / "share" / "camera_calibration" / "image").string();
    }
    // 3. 开发环境回退 (针对在工作空间根目录运行)
    else if (std::filesystem::exists(current_path / "src" / "camera_calibration" / "image")) {
        img_path = (current_path / "src" / "camera_calibration" / "image").string();
    } else {
        RCLCPP_FATAL(rclcpp::get_logger("main"), "找不到 'image' 文件夹！请确保你在正确的位置运行程序。当前路径: %s", current_path.c_str());
        return -1;
    }

    try {
        calibrator->run(img_path);
    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("main"), "主程序异常: %s", e.what());
        return -1;
    }

    rclcpp::shutdown();
    return 0;
}