# 添加可执行程序
add_executable(1_model_load 1_model_load.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_model_load ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)