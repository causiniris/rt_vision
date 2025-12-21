#ifndef PROCESSORS_BASE_HPP
#define PROCESSORS_BASE_HPP

#include <string>
#include <chrono>
#include <iostream>
#include <vector>

// 1. 处理结果结构体
template<typename T>
struct ProcessingResult {
    std::string operation;      // 操作名称
    std::string inputFile;      // 输入文件
    std::string outputFile;     // 输出路径
    bool success;               // 是否成功
    T additionalInfo;           // 附加信息（如错误信息或处理参数）
    double processingTimeMs;    // 处理耗时(ms)
};

// 2. 处理器模板基类
template<typename ImageType>
class ImageProcessor {
protected:
    std::string processorName_;
    
public:
    ImageProcessor(const std::string& name);
    virtual ~ImageProcessor() = default;
    
    // 纯虚函数：具体算法逻辑（由子类实现）
    virtual ProcessingResult<std::string> process(ImageType& image, 
                                                 const std::string& outputPath) = 0;
    
    std::string getName() const;

    // 模板方法：包含验证、计时、异常处理的标准流程
    ProcessingResult<std::string> processWithValidation(ImageType& image, 
                                                       const std::string& outputPath);
    
protected:
    // 预处理钩子（为了通用性，暂时返回 void，避免修改 Image 类）
    virtual void preprocess(const ImageType& image) {}
};

// 3. 模板方法实现

template<typename ImageType>
ImageProcessor<ImageType>::ImageProcessor(const std::string& name) 
    : processorName_(name) {}

template<typename ImageType>
std::string ImageProcessor<ImageType>::getName() const {
    return processorName_;
}

template<typename ImageType>
ProcessingResult<std::string> ImageProcessor<ImageType>::processWithValidation(ImageType& image, const std::string& outputPath) {
    ProcessingResult<std::string> result;
    result.operation = processorName_;
    result.inputFile = image.getFilename(); // 依赖 Image 类的接口
    result.outputFile = outputPath;
    result.processingTimeMs = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        // 1. 验证图片是否已加载
        if (!image.isLoaded()) {
            // 尝试自动加载
            if (!image.load()) {
                result.success = false;
                result.additionalInfo = "Validation Failed: Image not loaded and load() failed.";
                return result; 
            }
        }
        
        // 2. 调用预处理（可选）
        this->preprocess(image);
        
        // 3. 执行具体的处理逻辑
        result = this->process(image, outputPath);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.additionalInfo = std::string("Exception: ") + e.what();
    } catch (...) {
        result.success = false;
        result.additionalInfo = "Unknown Exception occurred.";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.processingTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    return result;
}

#endif