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

#Build unit test
add_executable(test_pmem ${CMAKE_SOURCE_DIR}/test/unit/test_pmem.cpp)
target_link_libraries(test_pmem pmemobj)
target_link_libraries(test_pmem msgpackc)
target_link_libraries(test_pmem boost_system boost_serialization)
target_link_libraries(test_pmem capnp)

#Build performance test
#add_subdirectory(test/performance)
