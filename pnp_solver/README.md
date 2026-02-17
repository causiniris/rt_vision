# RoboMaster PNP 解算模块

## 📖 1. 原理简述
PNP (Perspective-n-Point) 问题是指已知 $n$ 个 3D 空间点及其对应的 2D 图像投影点，求解相机与物体之间的位姿变换（旋转 $R$ 和平移 $t$）。

基本方程为：
$$s \begin{bmatrix} u \\ v \\ 1 \end{bmatrix} = K [R | t] \begin{bmatrix} X \\ Y \\ Z \\ 1 \end{bmatrix}$$

其中：
* $(u, v)$ 是像素坐标。
* $K$ 是相机内参矩阵。
* $X, Y, Z$ 是世界坐标。
* $s$ 是尺度因子（深度）。

本程序使用 `cv::solvePnP` 的 `ITERATIVE` 方法（基于 Levenberg-Marquardt 优化算法）求解该非线性最小二乘问题。

## 🚀 2. 编译与运行
```bash
colcon build --packages-select pnp_solver --symlink-install
source install/setup.bash
ros2 run pnp_solver pnp_solver_node