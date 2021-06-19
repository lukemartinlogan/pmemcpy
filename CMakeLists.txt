cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES C CXX)

include( FindPkgConfig )
include( CheckIncludeFileCXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

option(CMAKE_BUILD_TYPE Debug)
option(ALL_SERIALIZERS ON)
option(CAPNPROTO_ENABLED ON)
option(MSGPACK_ENABLED OFF)
option(BOOST_ENABLED OFF)
option(CEREAL_ENABLED OFF)

if ( CMAKE_BUILD_TYPE STREQUAL Debug)
    set( CMAKE_CXX_BUILD_FLAGS "-g -O0" )
elseif ( CMAKE_BUILD_TYPE STREQUAL Release )
    set( CMAKE_CXX_BUILD_FLAGS "-g -O3" )
endif()

pkg_check_modules( libpmemobj REQUIRED IMPORTED_TARGET )

if( NOT libpmemobj_FOUND )
    message( FATAL_ERROR "PMDK libraries not found" )
endif()

include_directories(include src/serialize)

#Build CapNP File
set(CAPNP_SRC_DIR ${CMAKE_SOURCE_DIR}/src/serialize)
set(CAPNP_PROTO_DIR ${CMAKE_SOURCE_DIR}/include/pmemcpy/serialize)
add_custom_command(
        OUTPUT ${CAPNP_SRC_DIR}/basic_capnp.capnp.c++
        COMMAND capnp compile --src-prefix=${CAPNP_PROTO_DIR} -oc++:${CAPNP_SRC_DIR} ${CAPNP_PROTO_DIR}/basic_capnp.capnp
        DEPENDS ${CAPNP_PROTO_DIR}/basic_capnp.capnp)

#Build the serialization library
add_library(pmemcpy SHARED ${CAPNP_SRC_DIR}/basic_capnp.capnp.c++)
target_link_libraries(pmemcpy pmemlog pmem pmemobj capnp boost_system boost_filesystem)

#Build unit test
set(TEST_LIBS pmemlog pmem pmemobj msgpackc boost_system boost_filesystem boost_serialization capnp kj mpi pthread)
add_executable(test_parallel_io ${CMAKE_SOURCE_DIR}/test/unit/test_parallel_io.cpp)
target_link_libraries(test_parallel_io ${TEST_LIBS})
add_executable(test_vector ${CMAKE_SOURCE_DIR}/test/unit/test_vector.cpp)
target_link_libraries(test_vector ${TEST_LIBS})
add_executable(test_pmem ${CMAKE_SOURCE_DIR}/test/unit/test_pmem.cpp)
target_link_libraries(test_pmem ${TEST_LIBS})

add_executable(test_pmemulator ${CMAKE_SOURCE_DIR}/test/unit/test_pmemulator.cpp)
target_link_libraries(test_pmemulator dl ${TEST_LIBS})
target_include_directories(test_pmemulator PUBLIC ${CMAKE_SOURCE_DIR}/test/performance/pmemulator)

#Build performance test and pmemulator
add_subdirectory(test/performance)

install(TARGETS pmemcpy LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
