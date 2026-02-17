#ifndef PNP_SOLVER_PNP_SOLVER_HPP_
#define PNP_SOLVER_PNP_SOLVER_HPP_

#include <opencv2/opencv.hpp>
#include <vector>
#include <optional> // C++17 特性

namespace robotac_vision {

// 定义 PNP 解算结果结构体，方便一次性返回所有需要的数据
struct PnPResult {
    cv::Mat rvec;           // 旋转向量 (3x1)
    cv::Mat tvec;           // 平移向量 (3x1)
    cv::Mat rotation_matrix;// 旋转矩阵 (3x3)
    double distance;        // 目标距离相机的直线距离 (L2范数)
    bool success;           // 解算是否成功
};

class PnPSolver {
public:
    /**
     * @brief 构造函数，初始化相机内参
     * @param camera_matrix 内参矩阵 (3x3)
     * @param dist_coeffs 畸变系数 (1x5 or 1x8)
     * @note 使用 const cv::Mat& 避免深拷贝，提高性能
     */
    PnPSolver(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs);
    ~PnPSolver() = default;

    /**
     * @brief 执行 PNP 解算
     * @param object_points 世界坐标系下的 3D 点 (单位: m 或 cm，需统一)
     * @param image_points  图像坐标系下的 2D 点 (单位: pixel)
     * @param method        OpenCV 解算方法，默认 ITERATIVE
     * @return PnPResult    包含位姿和距离的结果
     */
    PnPResult solve(const std::vector<cv::Point3f>& object_points,
                    const std::vector<cv::Point2f>& image_points,
                    int method = cv::SOLVEPNP_ITERATIVE);

private:
    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;
};

} // namespace robotac_vision

#endif // PNP_SOLVER_PNP_SOLVER_HPP_