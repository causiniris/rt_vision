#include "pnp_solver/pnp_solver.hpp"
#include <iostream>
#include <cmath>

namespace robotac_vision {

PnPSolver::PnPSolver(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs) {
    // 执行浅拷贝 (Reference counting)，开销极小
    // 如果担心外部修改 mat，可以使用 .clone() 进行深拷贝，但在高性能场景下，
    // 我们假设调用者遵守 const 契约。此处为了安全性 clone 一份内参以防外部释放。
    this->camera_matrix_ = camera_matrix.clone();
    this->dist_coeffs_ = dist_coeffs.clone();
}

PnPResult PnPSolver::solve(const std::vector<cv::Point3f>& object_points,
                           const std::vector<cv::Point2f>& image_points,
                           int method) {
    PnPResult result;
    result.success = false;

    // [鲁棒性] 边界条件检查
    if (object_points.size() != image_points.size()) {
        std::cerr << "[Error] 3D点与2D点数量不匹配！" << std::endl;
        return result;
    }
    if (object_points.size() < 4) {
        std::cerr << "[Error] 点数少于4，SOLVEPNP_ITERATIVE 可能不稳定！" << std::endl;
        return result;
    }
    if (camera_matrix_.empty()) {
        std::cerr << "[Error] 相机内参未初始化！" << std::endl;
        return result;
    }

    try {
        // [核心] 调用 OpenCV solvePnP
        // rvec: 旋转向量 (Compact representation of rotation)
        // tvec: 平移向量 (Translation from Camera to Object origin)
        bool pnp_status = cv::solvePnP(object_points, image_points, 
                                      camera_matrix_, dist_coeffs_, 
                                      result.rvec, result.tvec, 
                                      false, method);
        
        if (pnp_status) {
            result.success = true;

            // [原理] Rodrigues 变换：将旋转向量转换为旋转矩阵
            // 旋转向量的方向表示旋转轴，模长表示旋转角度
            cv::Rodrigues(result.rvec, result.rotation_matrix);

            // [原理] 计算距离：tvec 的 L2 范数 (x^2 + y^2 + z^2 的平方根)
            // tvec 表示物体坐标系原点在相机坐标系下的坐标
            result.distance = cv::norm(result.tvec); 
        }
    } catch (const cv::Exception& e) {
        // [鲁棒性] 捕获 OpenCV 内部可能抛出的计算错误（如矩阵奇异）
        std::cerr << "[Exception] OpenCV Error in solvePnP: " << e.what() << std::endl;
        result.success = false;
    }

    return result;
}

} // namespace robotac_vision