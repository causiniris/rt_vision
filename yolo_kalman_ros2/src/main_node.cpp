#include <rclcpp/rclcpp.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <cstdlib> // 需要这个头文件来获取环境变量
#include "yolo_kalman_ros2/YOLOv8Detector.hpp"
#include "yolo_kalman_ros2/KalmanTracker.hpp"

class YoloKalmanNode : public rclcpp::Node {
public:
    YoloKalmanNode() : Node("yolo_kalman_node") {
        // 1. 获取包安装路径（用于读取输入视频，因为编译时会把视频拷过去）
        std::string pkg_share = ament_index_cpp::get_package_share_directory("yolo_kalman_ros2");
        std::string input_video = pkg_share + "/video/test_1.mp4";

        // 2. [修改点] 设置输出路径为你的源码目录
        // 获取主目录 (即 /home/causin)
        const char* home_dir = std::getenv("HOME");
        if (home_dir == nullptr) {
            RCLCPP_ERROR(this->get_logger(), "无法获取 HOME 目录!");
            return;
        }
        // 拼接绝对路径：直接写到你的源码工程里
        std::string output_video = std::string(home_dir) + "/rt_vision/yolo_kalman_ros2/video/result.mp4";

        // 初始化模型
        detector_ = std::make_shared<YOLOv8Detector>(pkg_share + "/models/best.onnx");

        if (!cap_.open(input_video)) {
            RCLCPP_ERROR(this->get_logger(), "无法打开输入视频: %s", input_video.c_str());
            return;
        }

        int w = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
        int h = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
        double fps = cap_.get(cv::CAP_PROP_FPS);

        // 打开视频写入器
        writer_.open(output_video, cv::VideoWriter::fourcc('m','p','4','v'), fps, cv::Size(w, h));

        if (!writer_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "无法创建输出文件! 请检查路径是否存在: %s", output_video.c_str());
            return;
        }

        RCLCPP_INFO(this->get_logger(), "🚀 开始处理! 结果将保存至源码目录: %s", output_video.c_str());

        cv::Mat frame;
        int frame_count = 0;
        int total_frames = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_COUNT));

        while (cap_.read(frame)) {
            // 检测 (内部会画红框)
            auto detections = detector_->detect(frame);

            // 卡尔曼滤波预测 (内部会画绿点)
            for (size_t i = 0; i < detections.size(); ++i) {
                if (trackers_.size() <= i) {
                    trackers_.push_back(std::make_shared<KalmanTracker>());
                }
                
                // 获取中心点
                cv::Point center(detections[i].box.x + detections[i].box.width/2, 
                                 detections[i].box.y + detections[i].box.height/2);
                
                trackers_[i]->update(center);
                cv::Point pred = trackers_[i]->predict();

                // 绘制预测
                cv::circle(frame, pred, 8, cv::Scalar(0, 255, 0), -1);
                cv::putText(frame, "Kalman Prediction", cv::Point(pred.x + 10, pred.y - 10),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
            }

            // 写入视频
            writer_.write(frame);

            // 打印进度
            frame_count++;
            if (frame_count % 30 == 0) {
                float percent = (float)frame_count / total_frames * 100.0f;
                RCLCPP_INFO(this->get_logger(), "进度: %.1f%% (已处理 %d 帧)", percent, frame_count);
            }
        }

        RCLCPP_INFO(this->get_logger(), "✅ 处理完成！请直接去 rt_vision/yolo_kalman_ros2/video 文件夹查看 result.mp4");
        cap_.release();
        writer_.release();
    }

private:
    cv::VideoCapture cap_;
    cv::VideoWriter writer_;
    std::shared_ptr<YOLOv8Detector> detector_;
    std::vector<std::shared_ptr<KalmanTracker>> trackers_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<YoloKalmanNode>();
    rclcpp::shutdown();
    return 0;
}