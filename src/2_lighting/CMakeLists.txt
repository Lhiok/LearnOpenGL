# 添加可执行程序
add_executable(1_colors 1_colors.cpp)
add_executable(2_basic_lighting 2_basic_lighting.cpp)
add_executable(3_materials 3_materials.cpp)
add_executable(4_lighting_maps 4_lighting_maps.cpp)
add_executable(5_light_casters 5_light_casters.cpp)

# 链接lib glfw glad.c
target_link_libraries(1_colors ${GLFW3_LIBRARY} glad)
target_link_libraries(2_basic_lighting ${GLFW3_LIBRARY} glad)
target_link_libraries(3_materials ${GLFW3_LIBRARY} glad)
target_link_libraries(4_lighting_maps ${GLFW3_LIBRARY} glad)
target_link_libraries(5_light_casters ${GLFW3_LIBRARY} glad)