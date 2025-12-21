#ifndef CONCURRENT_BASE_HPP
#define CONCURRENT_BASE_HPP

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <iostream>
#include "rt_vision/observer_interface.hpp" // 【修改1】引入观察者接口定义
#include <algorithm> // 用于遍历

// 引入必要的依赖，因为我们要用到 Image 和 ProcessingResult
#include "image_base.hpp"
#include "processors_base.hpp"

// ==========================================
// 1. 新增：定义任务结构体
// ==========================================
struct Task {
    Image* image;                       // 待处理图片的指针
    ImageProcessor<Image>* processor;   // 使用的处理器指针
    std::string outputPath;             // 输出路径
};

// ==========================================
// 2. 线程安全队列模板类
// ==========================================
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    
public:
    void push(T value);
    bool pop(T& value);
    bool empty() const;
    size_t size() const;
};

// ==========================================
// 3. 图片处理管理器类
// ==========================================
class ImageProcessingManager {
private:
    // 资源所有权管理
    std::vector<std::unique_ptr<Image>> images_;
    std::vector<std::unique_ptr<ImageProcessor<Image>>> processors_;
    
    // 并发控制成员
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> isRunning_;
    
    // --- 新增的核心成员 ---
    ThreadSafeQueue<Task> taskQueue_;                     // 任务队列
    std::vector<ProcessingResult<std::string>> results_;  // 结果列表
    mutable std::mutex resultsMutex_;                     // 保护结果列表的互斥锁
    std::vector<std::shared_ptr<ProgressObserver>> observers_;
    mutable std::mutex observersMutex_;
    
public:
    ImageProcessingManager();
    ~ImageProcessingManager();
    
    void addImage(std::unique_ptr<Image> image);
    void addProcessor(std::unique_ptr<ImageProcessor<Image>> processor);
    void addObserver(std::shared_ptr<ProgressObserver> observer) {
        std::lock_guard<std::mutex> lock(observersMutex_);
        observers_.push_back(observer);
    }
    
    void startProcessing(int numThreads = 4);
    void stopProcessing();
    
    double getProgress() const;
    std::vector<ProcessingResult<std::string>> getResults() const;
    
private:
    void workerThread();
    // 辅助函数：如果不使用队列模式，可以使用这个处理单个任务（可选保留）
    void notifyTaskComplete(const ProcessingResult<std::string>& result) {
        std::lock_guard<std::mutex> lock(observersMutex_);
        for (auto& obs : observers_) {
            if (obs) {
                obs->onTaskCompleted(result);
                // 如果需要进度百分比，也可以在这里计算并调用 obs->onProgressUpdate(progress)
            }
        }
    }
    void processSingleTask(int imageIndex, int processorIndex);
};

// ==========================================
// 4. 模板方法的具体实现 (必须放在头文件中)
// ==========================================

template<typename T>
void ThreadSafeQueue<T>::push(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(value));
    condition_.notify_one(); // 唤醒一个等待的线程
}

template<typename T>
bool ThreadSafeQueue<T>::pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    // 这里使用非阻塞模式或简单的检查
    // 如果队列为空，直接返回 false
    if (queue_.empty()) {
        return false;
    }
    
    value = std::move(queue_.front());
    queue_.pop();
    return true;
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

#endif