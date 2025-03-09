# 添加可执行程序
add_executable(1_advanced_lighting 1_advanced_lighting.cpp)
add_executable(2_gamma_correction 2_gamma_correction.cpp)
add_executable(3_shadow_mapping 3_shadow_mapping.cpp)
add_executable(4_point_shadows 4_point_shadows.cpp)
add_executable(5_normal_mapping 5_normal_mapping.cpp)
add_executable(6_parallax_mapping 6_parallax_mapping.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_advanced_lighting ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(2_gamma_correction ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(3_shadow_mapping ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(4_point_shadows ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(5_normal_mapping ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)
target_link_libraries(6_parallax_mapping ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} glad)