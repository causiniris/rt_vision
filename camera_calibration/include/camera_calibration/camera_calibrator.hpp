#ifndef CAMERA_CALIBRATION_CAMERA_CALIBRATOR_HPP_
#define CAMERA_CALIBRATION_CAMERA_CALIBRATOR_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>

#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>

namespace robotac_vision {

class CameraCalibrator {
public:
    explicit CameraCalibrator(cv::Size board_size, float square_size);
    ~CameraCalibrator() = default;

    void run(const std::string& image_folder_path);

private:
    // 处理单帧：提取角点、亚像素优化
    bool processSingleImage(const cv::Mat& src, std::vector<cv::Point2f>& out_corners);

    // 执行标定
    void performCalibration();

    // 计算重投影误差
    double computeReprojectionError(
        const std::vector<std::vector<cv::Point3f>>& object_points,
        const std::vector<std::vector<cv::Point2f>>& image_points,
        const std::vector<cv::Mat>& rvecs,
        const std::vector<cv::Mat>& tvecs,
        const cv::Mat& camera_matrix,
        const cv::Mat& dist_coeffs);

    // 可视化矫正结果
    void visualizeRectification(const cv::Mat& src, const std::string& filename, const std::string& output_dir);

    cv::Size board_size_;
    float square_size_;
    cv::Size image_size_;

    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;

    std::vector<std::vector<cv::Point2f>> all_image_points_;
    std::vector<std::vector<cv::Point3f>> all_object_points_;
    std::vector<std::string> valid_image_paths_;

    rclcpp::Logger logger_;
};

} // namespace robotac_vision

#endif // CAMERA_CALIBRATION_CAMERA_CALIBRATOR_HPP_