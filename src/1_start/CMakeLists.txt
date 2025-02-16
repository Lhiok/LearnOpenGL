# 添加可执行程序
add_executable(1_hello_window 1_hello_window.cpp)
add_executable(2_hello_triangle 2_hello_triangle.cpp)
add_executable(3_shaders 3_shaders.cpp)
add_executable(4_textures 4_textures.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_hello_window ${GLFW3_LIBRARY} glad)
target_link_libraries(2_hello_triangle ${GLFW3_LIBRARY} glad)
target_link_libraries(3_shaders ${GLFW3_LIBRARY} glad)
target_link_libraries(4_textures ${GLFW3_LIBRARY} glad)