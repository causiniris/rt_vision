cmake_minimum_required(VERSION 3.8)
project(learning_tf2_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

# 对应文档 1.2: Add executable and link libraries
add_executable(turtle_tf2_broadcaster src/turtle_tf2_broadcaster.cpp)
ament_target_dependencies(turtle_tf2_broadcaster
  rclcpp
  tf2
  tf2_ros
  geometry_msgs
  turtlesim
)

# 对应文档 1.2: Install executable
install(TARGETS
  turtle_tf2_broadcaster
  DESTINATION lib/${PROJECT_NAME})

# 对应文档 2.3: Install launch directory
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

ament_package()