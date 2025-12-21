#include "rt_vision/concurrent_base.hpp"
#include <iostream>
#include <filesystem>

ImageProcessingManager::ImageProcessingManager() : isRunning_(false) {}

ImageProcessingManager::~ImageProcessingManager() {
    stopProcessing();
}

void ImageProcessingManager::addImage(std::unique_ptr<Image> image) {
    images_.push_back(std::move(image));
}

void ImageProcessingManager::addProcessor(std::unique_ptr<ImageProcessor<Image>> processor) {
    processors_.push_back(std::move(processor));
}

void ImageProcessingManager::startProcessing(int numThreads) {
    if (isRunning_) return;
    isRunning_ = true;

    // ==========================================
    // [新增] 1. 准备输出文件夹
    // ==========================================
    std::string outputDirName = "processed_result"; // 你想叫什么名字就在这里改
    try {
        if (!std::filesystem::exists(outputDirName)) {
            std::filesystem::create_directory(outputDirName);
            std::cout << "[Manager] Created output directory: " << outputDirName << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[Error] Failed to create output directory: " << e.what() << "\n";
    }

    // ==========================================
    // 2. 生产任务 (逻辑更新)
    // ==========================================
    for (const auto& img : images_) {
        for (const auto& proc : processors_) {
            Task task;
            task.image = img.get();
            task.processor = proc.get();
            
            // 获取原文件信息
            std::filesystem::path srcPath(img->getFilename());
            std::string stem = srcPath.stem().string();      // 文件名 (不含后缀)
            std::string ext = srcPath.extension().string();  // 后缀 (如 .jpg)
            
            // [修改] 组合新的输出路径: 文件夹/out_原名_操作名.后缀
            // 例如: processed_result/out_test_Resize.jpg
            std::filesystem::path outPath = std::filesystem::path(outputDirName) / 
                                            ("out_" + stem + "_" + proc->getName() + ext);
            
            task.outputPath = outPath.string(); // 转回 string 赋值给任务

            taskQueue_.push(task);
        }
    }

    std::cout << "[Manager] Tasks generated. Starting " << numThreads << " threads...\n";

    // 3. 启动工作线程
    for (int i = 0; i < numThreads; ++i) {
        workerThreads_.emplace_back(&ImageProcessingManager::workerThread, this);
    }
}

void ImageProcessingManager::stopProcessing() {
    isRunning_ = false;
    // 等待所有线程结束
    for (auto& t : workerThreads_) {
        if (t.joinable()) t.join();
    }
    workerThreads_.clear();
}

void ImageProcessingManager::workerThread() {
    while (isRunning_) {
        Task task;
        // 尝试获取任务
        if (taskQueue_.pop(task)) {
            // 1. 执行任务 (处理图片)
            auto result = task.processor->processWithValidation(*task.image, task.outputPath);
            
            // 2. 保存结果 (写入内存列表)
            {
                std::lock_guard<std::mutex> lock(resultsMutex_);
                results_.push_back(result);
            }
            
            // 3. 【核心修改】实时通知观察者！
            // 告诉外界："嘿，我搞定了一张图片！"
            notifyTaskComplete(result);
            
            // (可选) 原来的 std::cout 打印可以删掉，改由观察者在外部统一打印
            // std::cout << "  -> Processed: " << task.outputPath << "\n";

        } else {
            // 队列为空时的等待逻辑
            if (!isRunning_ && taskQueue_.empty()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

std::vector<ProcessingResult<std::string>> ImageProcessingManager::getResults() const {
    std::lock_guard<std::mutex> lock(resultsMutex_);
    return results_;
}

// 可选：如果不使用多线程，用于单线程调试
void ImageProcessingManager::processSingleTask(int imageIndex, int processorIndex) {
    // 简单实现略
}

double ImageProcessingManager::getProgress() const { return 0.0; }