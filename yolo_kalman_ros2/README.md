# YOLOv8 + Kalman Filter ROS 2 Package

这是一个基于 **ROS 2 Jazzy** 的视觉感知项目。它实现了使用 **YOLOv8 (ONNX Runtime)** 进行车辆目标检测，并结合 **卡尔曼滤波 (Kalman Filter)** 对车辆轨迹进行实时预测。

本项目专为虚拟机环境优化，采用**离线视频处理**模式，确保推理稳定并生成高帧率的标注视频。

## 🎯 项目功能 (Features)

* **目标检测 (Object Detection)**:
    * 使用 C++ 原生 ONNX Runtime 加载 YOLOv8 模型。
    * 手动解析 ONNX 张量输出 (支持 `[1, 84, 8400]` 格式)，支持 NMS（非极大值抑制）。
    * **可视化**: 红色矩形框 (Red Bounding Box)。
* **轨迹预测 (Trajectory Prediction)**:
    * 为每个检测到的目标分配独立的卡尔曼滤波器。
    * 基于匀速模型 (Constant Velocity Model) 预测下一帧位置。
    * **可视化**: 绿色实心圆点 (Green Dot) 及文字标注。
* **视频生成 (Video Generation)**:
    * 读取输入视频 (`test_1.mp4`)。
    * 处理完成后自动在源码目录下生成结果视频 (`result.mp4`)。

## 🛠️ 依赖环境 (Dependencies)

* **OS**: Ubuntu 24.04 (Noble Numbat)
* **ROS Distribution**: ROS 2 Jazzy Jalisco
* **Libraries**:
    * OpenCV 4.x (System default)
    * ONNX Runtime (C++ API, Linux-x64)
    * ROS 2 standard packages (`cv_bridge`, `image_transport`, `rclcpp`)

## 📂 目录结构 (Project Structure)

```text
yolo_kalman_ros2/
├── include/yolo_kalman_ros2/
│   ├── YOLOv8Detector.hpp    # YOLOv8 检测器头文件
│   └── KalmanTracker.hpp     # 卡尔曼滤波器头文件
├── src/
│   ├── main_node.cpp         # ROS 2 节点主逻辑 (视频处理与保存)
│   ├── YOLOv8Detector.cpp    # ONNX 推理、矩阵转置与 NMS 实现
│   └── KalmanTracker.cpp     # 卡尔曼预测与更新逻辑
├── models/
│   └── best.onnx             # YOLOv8 训练好的模型文件
├── video/
│   ├── test_1.mp4            # 输入测试视频
│   └── result.mp4            # (运行后生成) 输出结果视频
├── CMakeLists.txt            # 构建配置
└── package.xml               # 依赖描述
```

## 🚀 编译与运行 (Build & Run)
```bash
# 使用 colcon 编译
colcon build --symlink-install
#设置环境
source install/setup.bash
#运行节点
ros2 run yolo_kalman_ros2 yolo_kalman_node

#输出示例
[INFO] [yolo_kalman_node]: ✅ 模型加载成功: .../models/best.onnx
[INFO] [yolo_kalman_node]: 🚀 开始处理! 结果将保存至源码目录: .../video/result.mp4
[INFO] [Debug]: 📊 最高分: 0.88 | 对应类别ID: 2 (2=Car)
[INFO] [yolo_kalman_node]: 进度: 15.0% (已处理 150 帧)
...
[INFO] [yolo_kalman_node]: ✅ 处理完成！请直接去 rt_vision/yolo_kalman_ros2/video 文件夹查看 result.mp4
```
