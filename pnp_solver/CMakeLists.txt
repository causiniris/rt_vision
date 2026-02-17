cmake_minimum_required(VERSION 3.8)
project(pnp_solver)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# 开启编译器优化与警告
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

# 1. 生成库文件 (Modularization)
add_library(${PROJECT_NAME}_lib src/pnp_solver.cpp)
target_link_libraries(${PROJECT_NAME}_lib
  ${OpenCV_LIBS}
)

# 2. 生成节点可执行文件
add_executable(${PROJECT_NAME}_node src/main.cpp)
target_link_libraries(${PROJECT_NAME}_node
  ${PROJECT_NAME}_lib
  rclcpp::rclcpp
  ${OpenCV_LIBS}
)

# 3. 安装规则
install(TARGETS 
  ${PROJECT_NAME}_lib
  ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY include/ DESTINATION include)

ament_package()