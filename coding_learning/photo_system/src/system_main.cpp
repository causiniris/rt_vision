#include "rt_vision/system_main.hpp"
#include "rt_vision/image_formats.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>

void ImageManagementSystem::initialize() {
    std::cout << ">>> System Initialized <<<\n";
}

std::unique_ptr<Image> ImageManagementSystem::createImage(const std::string& filename) {
    std::string ext = std::filesystem::path(filename).extension().string();
    // 转小写
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".jpg" || ext == ".jpeg") {
        return std::make_unique<JPEGImage>(filename);
    } else if (ext == ".png") {
        return std::make_unique<PNGImage>(filename);
    } else if (ext == ".bmp") {
        return std::make_unique<BMPImage>(filename);
    }
    
    std::cerr << "[Error] Unsupported format: " << ext << "\n";
    return nullptr;
}

void ImageManagementSystem::processImageBatch(const std::vector<std::string>& imagePaths) {
    for (const auto& path : imagePaths) {
        auto img = createImage(path);
        if (img) {
            processingManager_.addImage(std::move(img));
        }
    }
    
    // 启动处理（4线程）
    processingManager_.startProcessing(4);
    
    // 简单等待机制：主线程等待 2 秒让子线程跑完
    // 实际项目中应该使用条件变量等待任务清空
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    processingManager_.stopProcessing();
}

void ImageManagementSystem::generateProcessingReport() {
    auto results = processingManager_.getResults();
    std::cout << "\n========== REPORT ==========\n";
    for (const auto& r : results) {
        std::cout << "[" << (r.success ? "OK" : "FAIL") << "] " 
                  << r.operation << " on " << r.inputFile 
                  << " (" << r.processingTimeMs << "ms)\n";
    }
    std::cout << "============================\n";
}