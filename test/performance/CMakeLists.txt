cmake_minimum_required( VERSION 3.8.0 )
project(e2e-hpdc2011 LANGUAGES CXX)

add_executable(read_1d_bp ${CMAKE_CURRENT_SOURCE_DIR}/1d/read_1d_bp.c)
target_link_libraries(read_1d_bp adiosread)
add_executable(read_1d_nc4 ${CMAKE_CURRENT_SOURCE_DIR}/1d/read_1d_nc4.c)
target_link_libraries(read_1d_bp adiosread)
add_executable(read_1d_nc5 ${CMAKE_CURRENT_SOURCE_DIR}/1d/read_1d_nc5.c)
add_executable(write_1d_bp ${CMAKE_CURRENT_SOURCE_DIR}/1d/write_1d_bp.c)
add_executable(write_1d_nc4 ${CMAKE_CURRENT_SOURCE_DIR}/1d/write_1d_nc4.c)
add_executable(write_1d_nc5 ${CMAKE_CURRENT_SOURCE_DIR}/1d/write_1d_nc5.c)

add_executable(read_1d_bp ${CMAKE_CURRENT_SOURCE_DIR}/1d/read_1d_bp.c)