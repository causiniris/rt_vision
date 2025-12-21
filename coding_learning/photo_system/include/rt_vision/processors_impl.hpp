#ifndef PROCESSORS_IMPL_HPP
#define PROCESSORS_IMPL_HPP

#include "processors_base.hpp"  // 包含上面的基类
#include "image_base.hpp"       // 包含 Image 类定义
#include <opencv2/opencv.hpp>
#include <cstring>              // for memcpy

// 辅助函数：将 Image 对象的数据转换为 cv::Mat
inline cv::Mat imageToMat(const Image& img) {
    // 1. 获取 Image 类中的原始像素数据
    std::vector<uint8_t> data = img.getPixelData();
    
    if (data.empty() || img.getWidth() <= 0 || img.getHeight() <= 0) {
        return cv::Mat();
    }
    
    // 2. 构造 cv::Mat (假设为 standard BGR 3通道)
    cv::Mat mat(img.getHeight(), img.getWidth(), CV_8UC3);
    
    // 安全检查：防止越界
    if (data.size() >= mat.total() * mat.elemSize()) {
        std::memcpy(mat.data, data.data(), mat.total() * mat.elemSize());
    } else {
        return cv::Mat(); // 数据大小不匹配
    }
    
    return mat;
}

// 1. 调整大小处理器 (ResizeProcessor)
class ResizeProcessor : public ImageProcessor<Image> {
private:
    int targetWidth_;
    int targetHeight_;

public:
    ResizeProcessor(int w, int h) 
        : ImageProcessor("Resize"), targetWidth_(w), targetHeight_(h) {}
        
    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        ProcessingResult<std::string> result;
        result.operation = getName();
        result.inputFile = image.getFilename();
        
        // 数据转换
        cv::Mat src = imageToMat(image);
        if (src.empty()) {
            result.success = false;
            result.additionalInfo = "Failed to convert Image to cv::Mat";
            return result;
        }
        
        // OpenCV 核心处理
        cv::Mat dst;
        cv::resize(src, dst, cv::Size(targetWidth_, targetHeight_));
        
        // 保存结果
        if (cv::imwrite(outputPath, dst)) {
            result.success = true;
            result.outputFile = outputPath;
            result.additionalInfo = "Resized to " + std::to_string(targetWidth_) + "x" + std::to_string(targetHeight_);
        } else {
            result.success = false;
            result.additionalInfo = "OpenCV imwrite failed";
        }
        
        return result;
    }
};

// 2. 旋转处理器 (RotateProcessor)
class RotateProcessor : public ImageProcessor<Image> {
private:
    double angle_;

public:
    RotateProcessor(double angle) 
        : ImageProcessor("Rotate"), angle_(angle) {}
        
    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        ProcessingResult<std::string> result;
        result.operation = getName();
        result.inputFile = image.getFilename();
        
        cv::Mat src = imageToMat(image);
        if (src.empty()) {
            result.success = false;
            result.additionalInfo = "Failed to convert Image to cv::Mat";
            return result;
        }
        
        // 计算旋转中心
        cv::Point2f center(src.cols / 2.0f, src.rows / 2.0f);
        cv::Mat rot = cv::getRotationMatrix2D(center, angle_, 1.0);
        
        // 调整边界框以防止裁剪
        cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle_).boundingRect2f();
        rot.at<double>(0, 2) += bbox.width / 2.0 - src.cols / 2.0;
        rot.at<double>(1, 2) += bbox.height / 2.0 - src.rows / 2.0;
        
        cv::Mat dst;
        cv::warpAffine(src, dst, rot, bbox.size());
        
        if (cv::imwrite(outputPath, dst)) {
            result.success = true;
            result.outputFile = outputPath;
            result.additionalInfo = "Rotated by " + std::to_string(angle_) + " degrees";
        } else {
            result.success = false;
            result.additionalInfo = "OpenCV imwrite failed";
        }
        
        return result;
    }
};

// 3. 格式转换处理器 (FormatConvertProcessor)
class FormatConverterProcessor : public ImageProcessor<Image> {
public:
    FormatConverterProcessor() : ImageProcessor("FormatConvert") {}
        
    ProcessingResult<std::string> process(Image& image, const std::string& outputPath) override {
        ProcessingResult<std::string> result;
        result.operation = getName();
        result.inputFile = image.getFilename();
        
        cv::Mat src = imageToMat(image);
        if (src.empty()) {
            result.success = false;
            result.additionalInfo = "Failed to convert Image to cv::Mat";
            return result;
        }
        
        // OpenCV 的 imwrite 会自动根据 outputPath 的扩展名（如 .png, .bmp）
        // 选择对应的编码器，从而实现格式转换。
        if (cv::imwrite(outputPath, src)) {
            result.success = true;
            result.outputFile = outputPath;
            result.additionalInfo = "Format converted based on output extension";
        } else {
            result.success = false;
            result.additionalInfo = "OpenCV imwrite failed";
        }
        
        return result;
    }
};

#endif