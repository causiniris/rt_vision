#ifndef YOLOV8_DETECTOR_HPP
#define YOLOV8_DETECTOR_HPP

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

struct Detection {
    cv::Rect box;
    float conf;
};

class YOLOv8Detector {
public:
    YOLOv8Detector(const std::string& model_path);
    std::vector<Detection> detect(cv::Mat& frame);

private:
    Ort::Env env;
    Ort::Session session{nullptr};
    std::vector<const char*> input_names = {"images"};
    std::vector<const char*> output_names = {"output0"};
};

#endif