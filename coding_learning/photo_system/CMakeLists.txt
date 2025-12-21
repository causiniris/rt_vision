cmake_minimum_required(VERSION 3.10)

# 项目名称
project(RT_Vision_System)

# === 1. 基础配置 ===
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 开启警告，保持代码质量
add_compile_options(-Wall)

# === 2. 查找依赖 ===
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# === 3. 头文件路径 (严格相对路径) ===
# 【核心修改点】：直接使用目录名 "include"，而不是 ${CMAKE_SOURCE_DIR}/include
# CMake 会自动将其解析为：当前 CMakeLists.txt 所在目录/include
# 这样无论你把 photo_system 文件夹移动到哪里，只要内部结构不变，它都能找到
include_directories(include)

# 如果你未来要用 external 里的 stb_image (如教程所示)，就把下面这行注释打开
include_directories(external)

# OpenCV 的路径通常由系统管理，这里直接引用变量
include_directories(${OpenCV_INCLUDE_DIRS})

# === 4. 收集源文件 (相对路径) ===
# 自动查找 src 目录下的所有 cpp 文件
# "src/*.cpp" 是相对于当前文件的相对路径
file(GLOB SOURCE_FILES "src/*.cpp")

# === 5. 生成目标 (相对路径) ===
# 指定 app/main.cpp 为入口，同样是相对路径
add_executable(photo_app 
    app/main.cpp 
    ${SOURCE_FILES}
)

# === 6. 链接库 ===
target_link_libraries(photo_app
    ${OpenCV_LIBS}
    Threads::Threads
)

# === 调试信息 ===
message(STATUS "Build setup for RT_Vision_System complete.")