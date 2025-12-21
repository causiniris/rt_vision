#ifndef OBSERVER_INTERFACE_HPP
#define OBSERVER_INTERFACE_HPP

// 包含处理器基类，因为我们需要用到 ProcessingResult 结构体
#include "rt_vision/processors_base.hpp"

// 观察者接口 (纯虚类)
// 任何想要接收进度通知的类（比如主界面、进度条组件）都需要继承它
class ProgressObserver {
public:
    virtual ~ProgressObserver() = default;

    // 进度更新回调 (0.0 - 1.0)
    virtual void onProgressUpdate(double progress) = 0;

    // 单个任务完成回调
    virtual void onTaskCompleted(const ProcessingResult<std::string>& result) = 0;
};

#endif