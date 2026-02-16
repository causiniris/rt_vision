cmake_minimum_required(VERSION 3.8)
project(camera_calibration)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV 4 REQUIRED)

include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
)

add_library(calibrator_lib
  src/camera_calibrator.cpp
)
target_link_libraries(calibrator_lib
  ${OpenCV_LIBS}
  rclcpp::rclcpp
)

add_executable(calibration_node src/main.cpp)
target_link_libraries(calibration_node
  calibrator_lib
  ${OpenCV_LIBS}
  rclcpp::rclcpp
)

install(TARGETS 
  calibrator_lib
  calibration_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

# 重要：安装 image 文件夹以便 ros2 run 找到
install(DIRECTORY image
  DESTINATION share/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_libraries(calibrator_lib)
ament_package()