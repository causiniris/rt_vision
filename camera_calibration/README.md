# Robotac 相机标定模块

战队专用高性能相机标定工具。基于 ROS 2 和 OpenCV 4 开发。

## ⚙️ 环境要求
* Ubuntu 22.04 / 24.04
* ROS 2 Humble / Jazzy
* OpenCV 4.x

## 🚀 编译指令
在工作空间根目录下执行：
```bash
colcon build --packages-select camera_calibration --symlink-install
source install/setup.bash
# 方式 1: 直接运行 (推荐调试用)
./build/camera_calibration/calibration_node

# 方式 2: ROS 2 Run (需注意相对路径问题)
ros2 run camera_calibration calibration_node
```
## 备注
如需测试其他摄像头，请将image文件夹下的图片替换为需要的，命名为calibration1,calibration2,...,calibration10