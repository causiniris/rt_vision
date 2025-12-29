#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Can not open camera!" << endl;
        return -1;
    }

    Mat kernel =
        getStructuringElement(MORPH_RECT, Size(5, 5));  // 定义形态学操作的核 (用于去噪，防止框闪烁)

    // 定义 FPS 计算相关的变量
    double t_start, t_end, fps;
    double t_freq = getTickFrequency();

    cout << "按 'q' 键退出程序..." << endl;

    while (true) {
        // [计时开始]
        t_start = (double)getTickCount();

        // 实时读取画面
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        // 颜色提取
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Mat mask_blue, mask_yellow, mask_combined;
        // 蓝色范围
        inRange(hsv, Scalar(78, 93, 57), Scalar(173, 255, 182), mask_blue);
        // 黄色范围
        inRange(hsv, Scalar(20, 214, 0), Scalar(58, 255, 255), mask_yellow);
        // 合并掩膜
        bitwise_or(mask_blue, mask_yellow, mask_combined);

        // 闭运算（膨胀+腐蚀）：这一步对视频流很重要，能让不稳定的色块变稳定
        morphologyEx(mask_combined, mask_combined, MORPH_CLOSE, kernel);

        // 轮廓检测 (findContours)
        vector<vector<Point>> contours;
        findContours(mask_combined, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 绘制边界框 (boundingRect)
        for (size_t i = 0; i < contours.size(); i++) {
            Rect box = boundingRect(contours[i]);
            if (box.area() < 800) continue;
            rectangle(frame, box, Scalar(0, 255, 0), 2);
            putText(frame, "Target", Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.6,
                    Scalar(0, 255, 0), 2);
        }

        // 计算并显示 FPS
        t_end = (double)getTickCount();
        fps   = t_freq / (t_end - t_start);
        // 将 FPS 显示在左上角 (红色字体)
        string fps_text = "FPS: " + to_string(int(fps));
        putText(frame, fps_text, Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        // 结果展示 (Concatenate & Imshow)
        Mat mask_bgr;
        cvtColor(mask_combined, mask_bgr, COLOR_GRAY2BGR);

        Mat result;
        hconcat(frame, mask_bgr, result);  // 左边是实景，右边是机器眼里的Mask

        imshow("Task 04 Real-time Tracking", result);
        if (waitKey(1) == 'q') break;
    }

    // 释放摄像头资源
    cap.release();
    destroyAllWindows();
    return 0;
}