#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

int main() {
    // 1. 读取图像
    Mat img = imread("test.png");
    if (img.empty()) {
        cout << "无法读取图片，请检查路径！" << endl;
        return -1;
    }

    // 2.颜色空间转换 (BGR -> HSV)
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    // 3. 颜色阈值处理
    Mat mask_blue, mask_yellow, mask_combined;
    inRange(hsv, Scalar(78, 93, 57), Scalar(173, 255, 182), mask_blue);

    inRange(hsv, Scalar(20, 214, 0), Scalar(58, 255, 255), mask_yellow);

    bitwise_or(mask_blue, mask_yellow, mask_combined);
    // 4. 轮廓检测
    vector<vector<Point>> contours;
    findContours(mask_combined, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 5. 绘制边界框
    for (size_t i = 0; i < contours.size(); i++) {
        Rect box = boundingRect(contours[i]);
        if (box.area() < 500) {
            continue;
        }
        rectangle(img, box, Scalar(0, 255, 0), 2);
        putText(img, "Target", Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5,
                Scalar(0, 255, 0), 1);
    }

    // 6.结果显示
    Mat mask_bgr;
    cvtColor(mask_combined, mask_bgr, COLOR_GRAY2BGR);

    Mat result;
    hconcat(img, mask_bgr, result);

    imshow("Task 02 Result", result);
    waitKey(0);

    return 0;
}