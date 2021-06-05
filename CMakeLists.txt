cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES CXX)

set( CMAKE_BUILD_TYPE "Debug" )
set( CMAKE_CXX_BUILD_FLAGS "-g -O0" )

include( FindPkgConfig )
include( CheckIncludeFileCXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

pkg_check_modules( libpmemobj REQUIRED IMPORTED_TARGET )

if( NOT libpmemobj_FOUND )
    message( FATAL_ERROR "PMDK libraries not found" )
endif()

include_directories(include)
#include_directories(include
#        /home/lukemartinlogan/spack/opt/spack/linux-linuxmint20-zen2/gcc-9.3.0/msgpack-c-3.1.1-wuob7l3wfkfrsry67wetwdbtxt4dzsjj/include
#        /home/lukemartinlogan/Documents/Projects/PhD/scspkg/packages/pmdk/include)

#Build unit test
add_executable(test_pmem ${CMAKE_SOURCE_DIR}/test/unit/test_pmem.cpp)
target_link_libraries(test_pmem pmemobj)
target_link_libraries(test_pmem msgpackc)

#Build performance test
#add_subdirectory(test/performance)
