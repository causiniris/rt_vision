cmake_minimum_required(VERSION 3.8)
project(ros_opencv_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. 寻找 ROS 依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)

# 2. 寻找 OpenCV (主要为了拿头文件路径)
# 强制指定去系统目录找 Config，防止找到你自己编译的那个
set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4/")
find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc imgcodecs)

# 3. 添加可执行文件
add_executable(vision_node src/vision_node.cpp)

# 4. 头文件包含
target_include_directories(vision_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
)

# 5. 设置依赖 (ROS 部分)
ament_target_dependencies(vision_node
  rclcpp
  std_msgs
  sensor_msgs
  cv_bridge
  image_transport
)

# =========================================================
# ⬇️ 核心修改：手动指定库文件的绝对路径 (专治 DSO missing) ⬇️
# =========================================================
target_link_libraries(vision_node
  # 强制链接系统自带的 OpenCV 库文件
  /usr/lib/x86_64-linux-gnu/libopencv_core.so
  /usr/lib/x86_64-linux-gnu/libopencv_highgui.so
  /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so
  /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so
)

# 6. 安装规则
install(TARGETS vision_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()