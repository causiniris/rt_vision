#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

// === 初始值 (目前是蓝色范围，想找黄色需要手动拖动滑块) ===
int h_min = 78, s_min = 93, v_min = 57;
int h_max = 173, s_max = 255, v_max = 255;

void on_trackbar(int, void*) {}

int main() {
    string video_path = "video_test.mp4";
    VideoCapture cap(video_path);

    if (!cap.isOpened()) {
        cout << "无法打开视频！" << endl;
        return -1;
    }

    // 调试窗口
    namedWindow("Debug", WINDOW_AUTOSIZE);
    createTrackbar("H Min", "Debug", &h_min, 179, on_trackbar);
    createTrackbar("H Max", "Debug", &h_max, 179, on_trackbar);
    createTrackbar("S Min", "Debug", &s_min, 255, on_trackbar);
    createTrackbar("S Max", "Debug", &s_max, 255, on_trackbar);
    createTrackbar("V Min", "Debug", &v_min, 255, on_trackbar);
    createTrackbar("V Max", "Debug", &v_max, 255, on_trackbar);

    Mat kernel_open  = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat kernel_close = getStructuringElement(MORPH_RECT, Size(7, 7));

    double t_start = 0;
    double fps     = 0;
    double t_freq  = getTickFrequency();

    // 强制控制播放速度 (33ms 约等于 30 FPS)
    int delay_ms = 33;

    cout << "========================================" << endl;
    cout << "操作说明：" << endl;
    cout << "1. 拖动滑块调整颜色识别范围。" << endl;
    cout << "2. 按键盘 's' 键 -> 将当前 HSV 值打印到这里。" << endl;
    cout << "3. 按键盘 'q' 键 -> 退出程序。" << endl;
    cout << "========================================" << endl;

    // 初始化计时器
    t_start = (double)getTickCount();

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cap.set(CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        // 缩小画面 (0.5倍)
        resize(frame, frame, Size(), 0.5, 0.5);

        Mat hsv, mask;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // 使用滑动条的值进行二值化
        inRange(hsv, Scalar(h_min, s_min, v_min), Scalar(h_max, s_max, v_max), mask);

        morphologyEx(mask, mask, MORPH_OPEN, kernel_open);
        morphologyEx(mask, mask, MORPH_CLOSE, kernel_close);

        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) {
            Rect box = boundingRect(contours[i]);
            if (box.area() < 400) continue;
            rectangle(frame, box, Scalar(0, 255, 0), 2);
            // 显示中心坐标
            string pos = to_string(box.x) + "," + to_string(box.y);
            putText(frame, pos, Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5,
                    Scalar(0, 255, 0), 1);
        }

        // 计算从"上一次循环开始"到现在的时间
        double t_now = (double)getTickCount();
        double dt    = (t_now - t_start) / t_freq;
        t_start      = t_now;  // 更新起点

        // 防止第一帧 dt 为 0
        if (dt > 0) fps = 1.0 / dt;

        string fps_text = "FPS: " + to_string(int(fps));
        putText(frame, fps_text, Point(20, 30), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2);

        imshow("Tracking", frame);
        imshow("Debug Mask", mask);

        // === 按键处理 ===
        int key = waitKey(delay_ms);  // 这里等待 33ms，确保视频看起来是 30 FPS

        if (key == 'q') break;
        if (key == 's') {
            // 按 s 键，打印当前的 HSV 值
            cout << "--------------------------------" << endl;
            cout << "当前 HSV 参数 (可复制到代码中):" << endl;
            cout << "Scalar(" << h_min << ", " << s_min << ", " << v_min << "), ";
            cout << "Scalar(" << h_max << ", " << s_max << ", " << v_max << ")" << endl;
            cout << "--------------------------------" << endl;
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}