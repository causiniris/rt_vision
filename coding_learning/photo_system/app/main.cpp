#include "rt_vision/system_main.hpp"
#include "rt_vision/processors_impl.hpp"
#include <iostream>
#include <vector>
#include <filesystem> // [新增] 引入文件系统库

namespace fs = std::filesystem;

int main() {
    // 1. 创建系统实例
    ImageManagementSystem sys;
    sys.initialize();

    // 2. 添加处理器 (和之前一样)
    sys.addProcessor<ResizeProcessor>(800, 600); // 缩放
    sys.addProcessor<RotateProcessor>(90.0);     // 旋转
    sys.addProcessor<FormatConverterProcessor>();// 格式转换

    // 3. [核心修改] 自动扫描 train1 文件夹
    std::vector<std::string> inputs;
    std::string targetFolder = "train1"; // 目标文件夹名字

    // 检查文件夹是否存在
    if (fs::exists(targetFolder) && fs::is_directory(targetFolder)) {
        std::cout << "[Main] Scanning folder: " << targetFolder << "..." << std::endl;
        
        // 遍历文件夹里的所有文件
        for (const auto& entry : fs::directory_iterator(targetFolder)) {
            // 只处理普通文件（过滤掉子文件夹）
            if (entry.is_regular_file()) {
                // 获取文件路径 (例如 "train1/abc.jpg")
                std::string path = entry.path().string();
                inputs.push_back(path);
                // std::cout << "  Found: " << path << std::endl;
            }
        }
    } else {
        std::cerr << "[Error] Folder '" << targetFolder << "' not found inside build directory!" << std::endl;
        return -1;
    }

    if (inputs.empty()) {
        std::cerr << "[Warning] No images found in " << targetFolder << std::endl;
        return 0;
    }

    // 4. 执行批处理
    sys.processImageBatch(inputs);

    // 5. 生成报告
    sys.generateProcessingReport();

    return 0;
}