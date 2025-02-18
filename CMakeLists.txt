# 指定cmake版本
cmake_minimum_required(VERSION 3.30...3.30)

# 项目名称
project(LearnOpenGL)

# 依赖环境
# 往CMAKE_MODULE_PATH追加一个目录 里面存放各种find文件 命名格式:Find{LIB}.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
# 调用FindGLFW3.cmake查找GLFW3头文件和lib 必须找到否则终止构建 找到设置GLFW3_XXX等系列变量
find_package(GLFW3 REQUIRED)
# 变量GLFW3_INCLUDE_DIR里存放着GLFW头文件的路径 然后添加头文件搜索目录
include_directories(${GLFW3_INCLUDE_DIR})
# 声明一个名为glad的lib 后面可以指定链接它 glad.c就会参与编译
add_library(glad include/glad.c)

# 添加头文件搜索目录 
include_directories(./include)

# 添加子目录
add_subdirectory(src/1_start)
add_subdirectory(src/2_lighting)