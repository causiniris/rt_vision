#include "camera_calibration/camera_calibrator.hpp"

namespace fs = std::filesystem;

namespace robotac_vision {

CameraCalibrator::CameraCalibrator(cv::Size board_size, float square_size)
    : board_size_(board_size), square_size_(square_size), logger_(rclcpp::get_logger("CameraCalibrator")) {
    camera_matrix_ = cv::Mat::eye(3, 3, CV_64F);
    dist_coeffs_ = cv::Mat::zeros(8, 1, CV_64F);
}

void CameraCalibrator::run(const std::string& image_folder_path) {
    RCLCPP_INFO(logger_, "正在扫描目录: %s", image_folder_path.c_str());

    // [新需求] 创建结果输出目录
    fs::path result_dir = fs::path(image_folder_path).parent_path() / "result";
    try {
        if (!fs::exists(result_dir)) {
            fs::create_directories(result_dir);
            RCLCPP_INFO(logger_, "已创建结果目录: %s", result_dir.c_str());
        }
    } catch (const fs::filesystem_error& e) {
        RCLCPP_ERROR(logger_, "无法创建结果目录: %s", e.what());
        return;
    }

    std::vector<std::string> target_files;
    for (int i = 1; i <= 10; ++i) {
        fs::path p = fs::path(image_folder_path) / ("calibration" + std::to_string(i) + ".jpg");
        target_files.push_back(p.string());
    }

    for (const auto& file_path : target_files) {
        if (!fs::exists(file_path)) {
            RCLCPP_WARN(logger_, "跳过丢失的文件: %s", file_path.c_str());
            continue;
        }

        cv::Mat src = cv::imread(file_path);
        if (src.empty()) {
            RCLCPP_ERROR(logger_, "无法解码图像: %s", file_path.c_str());
            continue;
        }

        if (image_size_.area() == 0) {
            image_size_ = src.size();
        } else if (src.size() != image_size_) {
            RCLCPP_WARN(logger_, "图像尺寸不一致，跳过: %s", file_path.c_str());
            continue;
        }

        std::vector<cv::Point2f> corners;
        if (processSingleImage(src, corners)) {
            std::vector<cv::Point3f> object_points;
            for (int i = 0; i < board_size_.height; ++i) {
                for (int j = 0; j < board_size_.width; ++j) {
                    object_points.emplace_back(j * square_size_, i * square_size_, 0);
                }
            }

            all_image_points_.push_back(corners);
            all_object_points_.push_back(object_points);
            valid_image_paths_.push_back(file_path);

            // [新需求] 可视化并保存角点检测结果
            cv::Mat debug_img = src.clone(); // 深拷贝一份用于绘制，不影响原图
            cv::drawChessboardCorners(debug_img, board_size_, corners, true);
            std::string filename = fs::path(file_path).filename().string();
            std::string debug_path = (result_dir / ("detected_" + filename)).string();
            cv::imwrite(debug_path, debug_img);
            RCLCPP_INFO(logger_, "已保存角点检测图: %s", debug_path.c_str());

        } else {
            RCLCPP_WARN(logger_, "未能提取角点: %s", fs::path(file_path).filename().c_str());
        }
    }

    if (all_image_points_.empty()) {
        RCLCPP_ERROR(logger_, "有效标定数据不足，程序终止。");
        return;
    }

    performCalibration();

    RCLCPP_INFO(logger_, "正在生成矫正对比图...");
    for (const auto& valid_path : valid_image_paths_) {
        cv::Mat img = cv::imread(valid_path);
        if (!img.empty()) {
            // 传入结果目录
            visualizeRectification(img, fs::path(valid_path).filename().string(), result_dir.string());
        }
    }
    RCLCPP_INFO(logger_, "🎉 所有结果已保存在: %s", result_dir.c_str());
}

bool CameraCalibrator::processSingleImage(const cv::Mat& src, std::vector<cv::Point2f>& out_corners) {
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    // 使用更严格的标志位组合以提高鲁棒性
    int flags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FILTER_QUADS;
    bool found = cv::findChessboardCorners(gray, board_size_, out_corners, flags);

    if (found) {
        // 亚像素精确化
        cv::cornerSubPix(gray, out_corners, cv::Size(11, 11), cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001));
    }
    return found;
}

void CameraCalibrator::performCalibration() {
    RCLCPP_INFO(logger_, "开始计算相机参数...");
    std::vector<cv::Mat> rvecs, tvecs;
    try {
        double rms = cv::calibrateCamera(all_object_points_, all_image_points_, image_size_, 
                                         camera_matrix_, dist_coeffs_, rvecs, tvecs);
        RCLCPP_INFO(logger_, "✅ 标定完成! RMS 误差: %.4f", rms);
        std::cout << "--- Camera Matrix ---\n" << camera_matrix_ << std::endl;
        std::cout << "--- Distortion Coeffs ---\n" << dist_coeffs_ << std::endl;
        double avg_err = computeReprojectionError(all_object_points_, all_image_points_, rvecs, tvecs, camera_matrix_, dist_coeffs_);
        RCLCPP_INFO(logger_, "平均重投影误差: %.4f 像素", avg_err);
    } catch (const cv::Exception& e) {
        RCLCPP_ERROR(logger_, "OpenCV 标定计算崩溃: %s", e.what());
    }
}

double CameraCalibrator::computeReprojectionError(
    const std::vector<std::vector<cv::Point3f>>& object_points,
    const std::vector<std::vector<cv::Point2f>>& image_points,
    const std::vector<cv::Mat>& rvecs,
    const std::vector<cv::Mat>& tvecs,
    const cv::Mat& camera_matrix,
    const cv::Mat& dist_coeffs) 
{
    double total_err = 0;
    double total_points_count = 0;
    for (size_t i = 0; i < object_points.size(); ++i) {
        std::vector<cv::Point2f> projected_points;
        cv::projectPoints(object_points[i], rvecs[i], tvecs[i], camera_matrix, dist_coeffs, projected_points);
        double err = cv::norm(image_points[i], projected_points, cv::NORM_L2);
        total_err += err * err;
        total_points_count += object_points[i].size();
    }
    return std::sqrt(total_err / total_points_count);
}

void CameraCalibrator::visualizeRectification(const cv::Mat& src, const std::string& filename, const std::string& output_dir) {
    cv::Mat dst_method1, dst_method2;
    
    // 方法一: Remap (推荐)
    cv::Mat map1, map2;
    cv::Mat new_cam_mat = cv::getOptimalNewCameraMatrix(camera_matrix_, dist_coeffs_, image_size_, 1, image_size_, 0);
    cv::initUndistortRectifyMap(camera_matrix_, dist_coeffs_, cv::Mat(), new_cam_mat, image_size_, CV_16SC2, map1, map2);
    cv::remap(src, dst_method1, map1, map2, cv::INTER_LINEAR);

    // 方法二: Undistort
    cv::undistort(src, dst_method2, camera_matrix_, dist_coeffs_);

    // 拼接对比
    cv::Mat canvas, small_src, small_dst1, small_dst2;
    float scale = 0.4; // 稍微调小一点以便查看
    cv::resize(src, small_src, cv::Size(), scale, scale);
    cv::resize(dst_method1, small_dst1, cv::Size(), scale, scale);
    cv::resize(dst_method2, small_dst2, cv::Size(), scale, scale);
    
    std::vector<cv::Mat> mats = {small_src, small_dst1, small_dst2};
    cv::hconcat(mats, canvas);

    // [新需求] 保存到 result 目录
    fs::path out_path = fs::path(output_dir) / ("rectified_" + filename);
    cv::imwrite(out_path.string(), canvas);
}

} // namespace robotac_vision