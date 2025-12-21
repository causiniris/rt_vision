#ifndef SYSTEM_MAIN_HPP
#define SYSTEM_MAIN_HPP

#include "image_base.hpp"
#include "processors_base.hpp"
#include "concurrent_base.hpp"
#include <vector>
#include <string>
#include <memory>
#include <iostream>

// 主系统类框架
class ImageManagementSystem {
private:
    ImageProcessingManager processingManager_;
    
public:
    void initialize();
    void processImageBatch(const std::vector<std::string>& imagePaths);
    void generateProcessingReport();
    
    // 工厂方法：根据文件后缀创建对应的 Image 对象
    std::unique_ptr<Image> createImage(const std::string& filename);
    
    // 模板方法：添加处理器
    template<typename ProcessorType, typename... Args>
    void addProcessor(Args&&... args);
};



template<typename ProcessorType, typename... Args>
void ImageManagementSystem::addProcessor(Args&&... args) {
    // 利用完美转发 (std::forward) 创建具体的处理器对象
    // 并将其所有权移交给管理器
    processingManager_.addProcessor(
        std::make_unique<ProcessorType>(std::forward<Args>(args)...)
    );
    std::cout << "[System] Added processor." << std::endl;
}

#endif