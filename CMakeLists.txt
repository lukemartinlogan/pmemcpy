cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES C CXX)

include( FindPkgConfig )
include( CheckIncludeFileCXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

option(CMAKE_BUILD_TYPE Debug)
option(BUILD_TESTS OFF)

set( CMAKE_C_FLAGS "-g -O3" )
set( CMAKE_CXX_FLAGS "-g -O3" )

pkg_check_modules( libpmemobj capnp REQUIRED IMPORTED_TARGET )
find_package(Boost)

include_directories(include src/serialize src/serialize/bp4)

#Build CapNP File
set(SERIALIZER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/serialize)
set(SERIALIZER_DST_DIR ${CMAKE_SOURCE_DIR}/include/pmemcpy/serialize)
add_custom_command(
        OUTPUT ${SERIALIZER_SRC_DIR}/basic_capnp.capnp.c++
        COMMAND capnp compile --src-prefix=${SERIALIZER_DST_DIR} -oc++:${SERIALIZER_SRC_DIR} ${SERIALIZER_DST_DIR}/basic_capnp.capnp
        DEPENDS ${SERIALIZER_DST_DIR}/basic_capnp.capnp)

#Build the serialization library
#add_library(pmemcpy SHARED ${SERIALIZER_SRC_DIR}/basic_capnp.capnp.c++ ${SERIALIZER_SRC_DIR}/bp4/BP4Base.cpp ${SERIALIZER_SRC_DIR}/bp4/BP4Deserializer.cpp ${SERIALIZER_SRC_DIR}/bp4/BP4Serializer.cpp)
add_library(pmemcpy SHARED ${SERIALIZER_SRC_DIR}/basic_capnp.capnp.c++)
target_link_libraries(pmemcpy pmem pmemobj capnp boost_system boost_filesystem)

if ( BUILD_TESTS )
  #Build unit test
  set(TEST_LIBS pmemlog pmem pmemobj msgpackc boost_system boost_filesystem boost_serialization capnp kj mpi pthread)
  add_executable(test_parallel_io ${CMAKE_SOURCE_DIR}/test/unit/test_parallel_io.cpp)
  target_link_libraries(test_parallel_io ${TEST_LIBS})
  add_executable(test_vector ${CMAKE_SOURCE_DIR}/test/unit/test_vector.cpp)
  target_link_libraries(test_vector ${TEST_LIBS})
  add_executable(test_pmem ${CMAKE_SOURCE_DIR}/test/unit/test_pmem.cpp)
  target_link_libraries(test_pmem ${TEST_LIBS})
  target_compile_definitions(test_pmem PUBLIC DEBUG)

  #API tests
  add_executable(test_hdf5 ${CMAKE_SOURCE_DIR}/test/api/test_hdf5.cpp)
  target_link_libraries(test_hdf5 mpi hdf5)
  add_executable(test_adios ${CMAKE_SOURCE_DIR}/test/api/test_adios.cpp)
  target_link_libraries(test_adios adios adiosread  mpi m pthread z zfp lz4 SZ blosc)

  #Build performance test
  add_subdirectory(test/performance)
endif()

install(TARGETS pmemcpy LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
