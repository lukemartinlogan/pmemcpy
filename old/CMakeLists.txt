cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES CXX)

set( CMAKE_BUILD_TYPE "Debug" )
set( CMAKE_CXX_BUILD_FLAGS "-g -O0" )

include( FindPkgConfig )
include( CheckIncludeFileCXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

pkg_check_modules( libpmem_pc libpmem2 libpmemobj REQUIRED IMPORTED_TARGET )

if( NOT libpmem_pc_FOUND )
  message( FATAL_ERROR "PMDK libraries not found" )
endif()

check_include_file_cxx( cereal/cereal.hpp HAVE_CEREAL_HPP )

add_executable( r1 r1.cc )
add_executable( w1 w1.cc )
add_executable( w2 w2.cc )
target_link_libraries( r1 PkgConfig::libpmem_pc )
target_link_libraries( w1 PkgConfig::libpmem_pc )
target_link_libraries( w2 PkgConfig::libpmem_pc )
  
