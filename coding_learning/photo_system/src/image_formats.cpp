#include "rt_vision/image_formats.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>

// ================= JPEG 实现 =================
JPEGImage::JPEGImage(const std::string& filename, int quality) 
    : Image(filename), quality_(quality) {}

bool JPEGImage::load() {
    cv::Mat img = cv::imread(filename_);
    if (img.empty()) return false;
    
    width_ = img.cols;
    height_ = img.rows;
    size_t size = img.total() * img.elemSize();
    pixelData_.resize(size);
    std::memcpy(pixelData_.data(), img.data, size);
    
    loaded_ = true;
    return onLoadSuccess();
}

bool JPEGImage::save(const std::string& outputPath) const {
    if (!loaded_) return false;
    cv::Mat img(height_, width_, CV_8UC3, (void*)pixelData_.data());
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, quality_};
    return cv::imwrite(outputPath, img, params);
}

std::vector<uint8_t> JPEGImage::getPixelData() const { return pixelData_; }
void JPEGImage::setQuality(int quality) { quality_ = quality; }
int JPEGImage::getQuality() const { return quality_; }
bool JPEGImage::onLoadSuccess() { return true; }

// ================= PNG 实现 =================
PNGImage::PNGImage(const std::string& filename, int compressionLevel) 
    : Image(filename), compressionLevel_(compressionLevel) {}

bool PNGImage::load() {
    cv::Mat img = cv::imread(filename_); // 默认读取为 BGR
    if (img.empty()) return false;
    
    width_ = img.cols;
    height_ = img.rows;
    size_t size = img.total() * img.elemSize();
    pixelData_.resize(size);
    std::memcpy(pixelData_.data(), img.data, size);
    
    loaded_ = true;
    return onLoadSuccess();
}

bool PNGImage::save(const std::string& outputPath) const {
    if (!loaded_) return false;
    cv::Mat img(height_, width_, CV_8UC3, (void*)pixelData_.data());
    std::vector<int> params = {cv::IMWRITE_PNG_COMPRESSION, compressionLevel_};
    return cv::imwrite(outputPath, img, params);
}

std::vector<uint8_t> PNGImage::getPixelData() const { return pixelData_; }
void PNGImage::setCompressionLevel(int level) { compressionLevel_ = level; }
int PNGImage::getCompressionLevel() const { return compressionLevel_; }
bool PNGImage::onLoadSuccess() { return true; }

// ================= BMP 实现 =================
BMPImage::BMPImage(const std::string& filename) : Image(filename) {}

bool BMPImage::load() {
    cv::Mat img = cv::imread(filename_);
    if (img.empty()) return false;
    width_ = img.cols;
    height_ = img.rows;
    size_t size = img.total() * img.elemSize();
    pixelData_.resize(size);
    std::memcpy(pixelData_.data(), img.data, size);
    loaded_ = true;
    return onLoadSuccess();
}

bool BMPImage::save(const std::string& outputPath) const {
    if (!loaded_) return false;
    cv::Mat img(height_, width_, CV_8UC3, (void*)pixelData_.data());
    return cv::imwrite(outputPath, img);
}

std::vector<uint8_t> BMPImage::getPixelData() const { return pixelData_; }
bool BMPImage::onLoadSuccess() { return true; }