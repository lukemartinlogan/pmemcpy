cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set( CMAKE_BUILD_TYPE "Debug" )
set( CMAKE_CXX_BUILD_FLAGS "-g -O0" )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

add_custom_target(build-pmemulator COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ext4 && make)
add_custom_target(clean-pmemulator COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ext4 && make clean)
add_custom_target(insert-pmemulator COMMAND sudo insmod ${CMAKE_CURRENT_SOURCE_DIR}/ext4/pmemulator.ko)
add_custom_target(rm-pmemulator COMMAND sudo rmmod pmemulator)
add_custom_target(check-pmemulator COMMAND lsmod | grep pmemulator)
add_custom_target(check-klog COMMAND dmesg)
add_custom_target(clean-klog COMMAND sudo dmesg --clear)