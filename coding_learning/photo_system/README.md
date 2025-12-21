# 📷 RT-Vision 智能图片管理系统 (Image Management System)

这是一个基于 C++17 和 OpenCV 开发的高性能图片批处理系统。该系统采用了现代 C++ 工程规范，支持多线程并发处理、多种图片格式读写以及可扩展的图像处理算法。

## ✨ 核心功能

* **多格式支持**：支持 JPG, PNG, BMP 格式的自动识别与读写 (基于工厂模式)。
* **并发处理**：内置线程池与任务队列，支持多线程并行处理海量图片。
* **图像算法**：
    * 📏 **Resize**: 智能调整图片大小。
    * 🔄 **Rotate**: 图像旋转 (保持完整画面不裁剪)。
    * 🔀 **Format Convert**: 格式自动转换 (如 jpg 转 png)。
* **设计模式**：应用了观察者模式 (Observer)、工厂模式 (Factory)、策略模式 (Strategy) 和 RAII 资源管理。

## 📂 项目结构

```text
photo_system/
├── app/
│   └── main.cpp             # 程序入口 (自动扫描与批处理逻辑)
├── src/
│   ├── image_formats.cpp    # 具体图片格式实现 (OpenCV 封装)
│   ├── manager_impl.cpp     # 线程池与任务分发逻辑
│   └── system_main.cpp      # 系统主控与工厂模式实现
├── include/
│   └── rt_vision/           # 头文件 (命名空间隔离)
│       ├── image_base.hpp   # 图片基类接口
│       ├── processors_*.hpp # 处理器模板与实现
│       ├── concurrent_*.hpp # 线程安全队列与管理器
│       └── ...
├── external/                # 第三方库预留 (stb_image)
├── CMakeLists.txt           # 构建脚本
└── README.md                # 项目说明文档
```

## 🛠️ 环境依赖 (Dependencies)

本项目基于 Linux 环境开发，推荐使用 Ubuntu 20.04 / 22.04 / 24.04。

### 核心依赖库
* **C++ 标准**: C++17 (必须支持 `std::filesystem`)
* **OpenCV**: 4.x 版本 (用于图像读写与处理算法)
* **CMake**: 3.10 或更高版本
* **编译器**: GCC 9+ 或 Clang 10+
* **Threads**: POSIX Threads (pthread)

### 📦 快速安装命令 (Ubuntu/Debian)

如果你使用的是 Ubuntu，可以直接运行以下命令一键配置环境：

```bash
# 1. 更新软件源
sudo apt update

# 2. 安装编译器和构建工具 (GCC, G++, Make, CMake)
sudo apt install build-essential cmake

# 3. 安装 OpenCV 开发包 (核心依赖)
# 注意：这会安装 OpenCV 4.x 及其头文件
sudo apt install libopencv-dev
```