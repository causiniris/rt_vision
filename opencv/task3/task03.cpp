#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>  // 用于存放拼接的图片列表

using namespace cv;
using namespace std;

int main() {
    // 1.imread
    Mat src = imread("../task2/test.png");
    if (src.empty()) {
        cout << "无法读取图片！" << endl;
        return -1;
    }

    // 2.BGR->Gray
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // 3.GaussianBlur
    Mat blur_img;
    GaussianBlur(gray, blur_img, Size(5, 5), 0);

    // 4.threshold
    Mat binary;
    threshold(blur_img, binary, 127, 255, THRESH_BINARY);

    // 5.erode/dilate
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat morph_img;
    erode(binary, morph_img, kernel);
    // dilate(binary, morph_img, kernel);

    // 6.hconcat
    Mat result;
    vector<Mat> images;
    images.push_back(blur_img);
    images.push_back(binary);
    images.push_back(morph_img);

    hconcat(images, result);  // 水平拼接

    // (可选) 为了在显示时能分清谁是谁，可以在图上写字
    putText(result, "2.Blur", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 2);
    putText(result, "3.Threshold", Point(blur_img.cols + 10, 30), FONT_HERSHEY_SIMPLEX, 1,
            Scalar(127), 2);
    putText(result, "4.Erode", Point(blur_img.cols * 2 + 10, 30), FONT_HERSHEY_SIMPLEX, 1,
            Scalar(127), 2);

    // 7.imshow
    imshow("Task 03 Process", result);

    waitKey(0);
    return 0;
}