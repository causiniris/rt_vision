cmake_minimum_required(VERSION 3.8)
project(yolo_kalman_ros2)

# ONNX Runtime 路径配置
set(ONNXRUNTIME_ROOT "/home/causin/libs/onnxruntime-linux-x64-1.16.3")

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# 查找依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(image_transport REQUIRED)

include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
  ${ONNXRUNTIME_ROOT}/include
)

# 定义可执行文件
add_executable(yolo_kalman_node
  src/main_node.cpp
  src/YOLOv8Detector.cpp
  src/KalmanTracker.cpp
)

# 链接依赖
ament_target_dependencies(yolo_kalman_node
  rclcpp
  sensor_msgs
  cv_bridge
  ament_index_cpp
  image_transport
)

target_link_libraries(yolo_kalman_node
  ${OpenCV_LIBS}
  ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
)

# 设置 RPATH 确保运行时能找到 so 库
set_target_properties(yolo_kalman_node PROPERTIES
  INSTALL_RPATH "${ONNXRUNTIME_ROOT}/lib"
  BUILD_RPATH   "${ONNXRUNTIME_ROOT}/lib"
)

# 安装规则
install(TARGETS yolo_kalman_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch models video DESTINATION share/${PROJECT_NAME})

ament_package()