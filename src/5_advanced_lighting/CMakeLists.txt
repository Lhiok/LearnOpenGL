# 添加可执行程序
add_executable(1_advanced_lighting 1_advanced_lighting.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_advanced_lighting ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)