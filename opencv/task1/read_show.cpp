#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    // 1.read
    Mat img = imread("test.png");

    // 2.transform BGR-gray
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // 3.gray-"BGR"
    Mat gray_img;
    cvtColor(gray, gray_img, COLOR_GRAY2BGR);

    // 4.hconcat
    Mat result;
    hconcat(img, gray_img, result);

    // 5.show
    imshow("Original vs Gray", result);

    waitKey(0);
    return 0;
}
