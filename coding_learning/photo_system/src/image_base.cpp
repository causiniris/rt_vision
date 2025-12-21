#include "rt_vision/image_base.hpp"
#include <iostream>

// 1. 基类构造函数实现
Image::Image(const std::string& filename) 
    : filename_(filename), width_(0), height_(0), loaded_(false) {}

// 2. "模板方法模式"的核心逻辑实现
bool Image::loadWithValidation() {
    // 先尝试加载
    if (this->load()) {
        // 加载成功后，执行钩子函数
        return this->onLoadSuccess();
    }
    return false;
}

// 3. 默认的加载成功回调
bool Image::onLoadSuccess() {
    // 可以在这里打印日志，或者更新一些状态
    // std::cout << "[Base] Image loaded: " << filename_ << std::endl;
    return true; 
}