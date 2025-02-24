# 添加可执行程序
add_executable(1_depth_testing 1_depth_testing.cpp)
add_executable(2_stencil_testing 2_stencil_testing.cpp)
add_executable(3_blending 3_blending.cpp)
add_executable(4_face_culling 4_face_culling.cpp)
add_executable(5_frame_buffers 5_frame_buffers.cpp)
add_executable(6_cubemaps 6_cubemaps.cpp)
add_executable(7_advanced_data 7_advanced_data.cpp)
add_executable(8_advanced_glsl 8_advanced_glsl.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_depth_testing ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(2_stencil_testing ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(3_blending ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(4_face_culling ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(5_frame_buffers ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(6_cubemaps ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(7_advanced_data ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(8_advanced_glsl ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)