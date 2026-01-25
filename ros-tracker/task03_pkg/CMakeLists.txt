cmake_minimum_required(VERSION 3.8)
project(task03_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# 1. 生成自定义消息
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/TrackerParams.msg"
)

# 2. 编译 Founder
add_executable(founder src/founder.cpp)
ament_target_dependencies(founder rclcpp std_msgs)
rosidl_target_interfaces(founder ${PROJECT_NAME} "rosidl_typesupport_cpp")

# 3. 编译 Tracker
add_executable(tracker src/tracker.cpp)
ament_target_dependencies(tracker rclcpp std_msgs)
rosidl_target_interfaces(tracker ${PROJECT_NAME} "rosidl_typesupport_cpp")

# 4. 安装规则 (把所有生成的东西装到 install 目录)
install(TARGETS
  founder
  tracker
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()