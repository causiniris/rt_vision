#include "yolo_kalman_ros2/KalmanTracker.hpp"

KalmanTracker::KalmanTracker() : kf(4, 2, 0) {
    // 状态转移矩阵 (匀速直线运动模型)
    kf.transitionMatrix = (cv::Mat_<float>(4, 4) << 1,0,1,0, 0,1,0,1, 0,0,1,0, 0,0,0,1);
    // 测量矩阵
    kf.measurementMatrix = (cv::Mat_<float>(2, 4) << 1,0,0,0, 0,1,0,0);
    // 对标你的 Python 代码: np.eye(4) * 0.03
    cv::setIdentity(kf.processNoiseCov, cv::Scalar::all(0.03));
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(kf.errorCovPost, cv::Scalar::all(1));
}

cv::Point KalmanTracker::predict() {
    cv::Mat p = kf.predict();
    return cv::Point(static_cast<int>(p.at<float>(0)), static_cast<int>(p.at<float>(1)));
}

void KalmanTracker::update(cv::Point p) {
    cv::Mat m = (cv::Mat_<float>(2, 1) << (float)p.x, (float)p.y);
    kf.correct(m);
}