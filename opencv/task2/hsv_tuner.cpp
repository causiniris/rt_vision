#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int hmin = 0, hmax = 180;
int smin = 0, smax = 255;
int vmin = 0, vmax = 255;

void on_trackbar(int, void*) {}

int main() {
    Mat frame = imread("test.png");
    if (frame.empty()) {
        cout << "找不到图片 task01_test.jpg，请检查路径！" << endl;
        return -1;
    }

    // 为了防止图片太大屏幕放不下，可以缩放一下 (可选)
    // resize(frame, frame, Size(640, 480));

    namedWindow("Trackbars", WINDOW_AUTOSIZE);

    createTrackbar("H Min", "Trackbars", &hmin, 180, on_trackbar);
    createTrackbar("H Max", "Trackbars", &hmax, 180, on_trackbar);
    createTrackbar("S Min", "Trackbars", &smin, 255, on_trackbar);
    createTrackbar("S Max", "Trackbars", &smax, 255, on_trackbar);
    createTrackbar("V Min", "Trackbars", &vmin, 255, on_trackbar);
    createTrackbar("V Max", "Trackbars", &vmax, 255, on_trackbar);

    while (true) {
        Mat hsv, mask;

        // --- 修改点 B：因为是静态图，不需要 cap >> frame，直接用读进来的 frame ---

        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);
        inRange(hsv, lower, upper, mask);

        imshow("Original", frame);
        imshow("Mask (Adjust these sliders!)", mask);

        if (waitKey(30) == 'q') break;
    }

    // 退出时打印结果
    cout << "--- 记下这些数字 ---" << endl;
    cout << "Scalar(" << hmin << ", " << smin << ", " << vmin << "), ";
    cout << "Scalar(" << hmax << ", " << smax << ", " << vmax << ")" << endl;

    return 0;
}