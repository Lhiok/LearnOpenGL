# 添加可执行程序
add_executable(1_hello_window 1_hello_window.cpp)
add_executable(2_hello_triangle 2_hello_triangle.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_hello_window ${GLFW3_LIBRARY} glad)
target_link_libraries(2_hello_triangle ${GLFW3_LIBRARY} glad)