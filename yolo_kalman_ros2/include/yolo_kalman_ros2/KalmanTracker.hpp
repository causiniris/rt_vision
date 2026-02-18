#ifndef KALMAN_TRACKER_HPP
#define KALMAN_TRACKER_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

class KalmanTracker {
public:
    KalmanTracker();
    
    // 修正：声明不带参数的 predict 和 接收 cv::Point 的 update
    cv::Point predict();
    void update(cv::Point p);

private:
    cv::KalmanFilter kf; // 直接使用对象，不使用指针，更简单稳定
};

#endif