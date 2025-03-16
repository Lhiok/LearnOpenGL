# 添加可执行程序
add_executable(1_text_rendering 1_text_rendering.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_text_rendering ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE2_LIBRARY} glad)