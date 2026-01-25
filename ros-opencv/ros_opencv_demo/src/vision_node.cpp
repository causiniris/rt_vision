#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <vector>

using namespace cv;
using namespace std;

class CubeColorDetector : public rclcpp::Node {
public:
    CubeColorDetector() : Node("vision_node") {
        sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            "image_raw", 10,
            std::bind(&CubeColorDetector::image_callback, this, std::placeholders::_1));

        pub_ = this->create_publisher<sensor_msgs::msg::Image>("result_image", 10);

        RCLCPP_INFO(this->get_logger(), "视觉节点已启动！正在执行你的 C++ 原始逻辑 (Task 02)...");
    }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        // 1. ROS -> OpenCV 格式转换
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        Mat img = cv_ptr->image;
        Mat hsv;

        // 2. 颜色空间转换 (BGR -> HSV)
        cvtColor(img, hsv, COLOR_BGR2HSV);

        // 3. 颜色阈值处理 (完全照搬你的截图参数)
        Mat mask_blue, mask_yellow, mask_combined;

        // 蓝色参数: 78-173, 93-255, 57-182
        inRange(hsv, Scalar(78, 93, 57), Scalar(173, 255, 182), mask_blue);

        // 黄色参数: 20-58, 214-255, 0-255
        inRange(hsv, Scalar(20, 214, 0), Scalar(58, 255, 255), mask_yellow);

        // 合并两个掩码 (bitwise_or)
        bitwise_or(mask_blue, mask_yellow, mask_combined);

        // 4. 轮廓检测
        vector<vector<Point>> contours;
        findContours(mask_combined, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 5. 绘制边界框
        int count = 0;
        for (size_t i = 0; i < contours.size(); i++) {
            Rect box = boundingRect(contours[i]);

            // 逻辑复刻：只过滤面积小于 500 的
            if (box.area() < 500) {
                continue;
            }

            // 绘制：绿色框，粗细 2
            rectangle(img, box, Scalar(0, 255, 0), 2);
            // 文字：Target
            putText(img, "Target", Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5,
                    Scalar(0, 255, 0), 1);

            count++;
        }

        // 6. 发布结果
        sensor_msgs::msg::Image::SharedPtr out_msg = cv_ptr->toImageMsg();
        pub_->publish(*out_msg);

        // 打印一下检测到的数量，方便确认
        // RCLCPP_INFO(this->get_logger(), "当前帧检测到目标数量: %d", count);
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CubeColorDetector>());
    rclcpp::shutdown();
    return 0;
}