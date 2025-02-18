# 添加可执行程序
add_executable(1_colors 1_colors.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_colors ${GLFW3_LIBRARY} glad)