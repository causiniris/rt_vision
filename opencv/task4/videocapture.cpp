#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

int main() {
    string video_path = "video_test.mp4";
    VideoCapture cap(video_path);

    if (!cap.isOpened()) {
        cout << "无法打开视频文件！" << endl;
        return -1;
    }

    // === 1. 标定参数 ===
    Scalar blue_min(75, 11, 72);
    Scalar blue_max(173, 223, 221);

    Scalar yellow_min(28, 25, 56);
    Scalar yellow_max(56, 255, 238);

    Mat kernel_open  = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat kernel_close = getStructuringElement(MORPH_RECT, Size(7, 7));

    // FPS 相关变量
    double t_freq  = getTickFrequency();
    double t_start = 0;
    double fps     = 0;

    // 目标帧率 30 FPS -> 每帧约 33ms
    const int TARGET_FPS   = 30;
    const int TARGET_DELAY = 1000 / TARGET_FPS;

    cout << "程序运行中... (按 'q' 退出)" << endl;

    while (true) {
        // [计时起点]：记录处理开始的时间
        int64 t_frame_start = getTickCount();

        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cap.set(CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        // 2. 缩小画面
        resize(frame, frame, Size(), 0.5, 0.5);

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // 3. 颜色提取
        Mat mask_blue, mask_yellow, mask_combined;
        inRange(hsv, blue_min, blue_max, mask_blue);
        inRange(hsv, yellow_min, yellow_max, mask_yellow);
        bitwise_or(mask_blue, mask_yellow, mask_combined);

        // 4. 形态学处理
        morphologyEx(mask_combined, mask_combined, MORPH_OPEN, kernel_open);
        morphologyEx(mask_combined, mask_combined, MORPH_CLOSE, kernel_close);

        // 5. 轮廓检测
        vector<vector<Point>> contours;
        findContours(mask_combined, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) {
            Rect box = boundingRect(contours[i]);
            if (box.area() < 400) continue;
            rectangle(frame, box, Scalar(0, 255, 0), 2);
            putText(frame, "Target", Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5,
                    Scalar(0, 255, 0), 1);
        }

        // 6. 显示结果
        // 为了显示更准的实时FPS，我们用“当前帧耗时”来估算，或者继续用上一帧的dt
        // 这里简单处理：显示上一帧算出的 FPS
        string fps_text = "FPS: " + to_string(int(fps));
        putText(frame, fps_text, Point(20, 30), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2);
        imshow("Result", frame);

        // ==========================================
        // 核心修改：智能计算 delay 时间
        // ==========================================
        int64 t_frame_end = getTickCount();
        // 计算处理这一帧花了多少毫秒
        double process_time_ms = (t_frame_end - t_frame_start) * 1000.0 / t_freq;

        // 剩余需要等待的时间 = 目标时间(33ms) - 已经消耗的时间
        int wait_ms = TARGET_DELAY - (int)process_time_ms;

        // 如果处理太慢，最少也要等 1ms 让 imshow 刷新；如果处理快，就等多一点补齐 33ms
        if (wait_ms < 1) wait_ms = 1;

        // 计算真实的 FPS (用于显示)
        // 真实一帧的总时间 = 处理时间 + 等待时间
        double total_frame_time_ms = process_time_ms + wait_ms;
        if (total_frame_time_ms > 0) fps = 1000.0 / total_frame_time_ms;

        // 打印调试（如果不想要刷屏，可以注释掉这行）
        // cout << "Process: " << process_time_ms << "ms | Wait: " << wait_ms << "ms | FPS: " << fps
        // << endl;

        if (waitKey(wait_ms) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}