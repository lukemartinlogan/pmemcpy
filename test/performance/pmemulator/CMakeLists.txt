cmake_minimum_required( VERSION 3.8.0 )
project(pm LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set( CMAKE_BUILD_TYPE "Debug" )
set( CMAKE_CXX_BUILD_FLAGS "-g -O0" )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

set(POSIX_SYSCALLS open __open_2 open64 openat openat64 creat dup dup2 dup3 mkstemp mkostemp mkstemps mkostemps
    read write pread pwrite pread64 pwrite64 readv preadv preadv64 preadv2 preadv64v2 writev pwritev pwritev64 pwritev2
    pwritev64v2 lseek lseek64 __xstat __xstat64 __lxstat __lxstat64 __fxstat __fxstat64 mmap mmap64 fsync fdatasync close
    aio_read aio_write aio_read64 aio_write64 aio_return aio_return64 lio_listio lio_listio64 rename)
set(SYSCALL_WRAP "-Wl,")
foreach(SYSCALL IN LISTS POSIX_SYSCALLS)
    set(SYSCALL_WRAP "${SYSCALL_WRAP},--wrap=${SYSCALL}")
endforeach()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(pmemulator SHARED pmemulator-posix.cpp)
target_link_libraries(pmemulator dl)
set_target_properties(pmemulator PROPERTIES COMPILE_FLAGS "${SYSCALL_WRAP}")
target_compile_definitions(pmemulator PUBLIC DEBUG)

install(TARGETS pmemulator LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES "pmemulator.h" DESTINATION ${CMAKE_INSTALL_PREFIX}/include)