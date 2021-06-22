//
// Created by lukemartinlogan on 5/30/21.
//

#ifndef PMEMCPY_ERRORS_H
#define PMEMCPY_ERRORS_H

#include <pmemcpy/util/error.h>

namespace pmemcpy {
    const Error FILE_NOT_FOUND(0, "File not found at {}");
    const Error INVALID_STORAGE_TYPE(1, "{} is not a valid storage method");
    const Error INVALID_SERIALIZER_TYPE(2, "{} is not a valid serializer type");
    const Error INVALID_TRANSPORT_TYPE(3, "{} is not a valid transport type");

    const Error POSIX_PATH_NOT_DIRECTORY(100, "{} must be a directory for POSIX storage type to work");
    const Error POSIX_READ_FAILED(101, "Did not complete read of {} bytes to {}/{}. Return value: {}. Error Message: {}.");
    const Error POSIX_WRITE_FAILED(102, "Did not complete write of {} bytes to {}/{}. Return value: {}. Error Message: {}.");
    const Error POSIX_TRUNCATE_FAILED(103, "Could not truncate {}/{} to size {}. Error Message: {}.");
    const Error POSIX_OPEN_FAILED(104, "Could not open {}/{}. Error Message: {}.");
    const Error POSIX_MMAP_FAILED(105, "Could not mmap {} from fd {}. Error Message: {}.");
    const Error POSIX_FIND_FAILED(106, "Could not find {}/{}. Error Message: {}.");

    const Error PMDK_HASH_POOL_TOO_SMALL(200, "{} bytes is too small. {} buckets requires at least {} bytes of space.");
    const Error PMDK_HASH_POOL_FAILED(201, "Could not allocate pool at {} of size {} with {} buckets. Error Message: {}.");
    const Error PMDK_CREATE_HEADER_FAILED(202, "Failed to allocate header for {} of size {} with {} buckets. Error Message: {}.");
    const Error PMDK_CANT_OPEN_POOL(204, "Failed to open pool at {}. Error Message: {}.");
    const Error PMDK_CANT_FIND_KEY(205, "Could not find key {} in the pool.");
    const Error PMDK_CANT_ALLOCATE_STR(206, "Could not allocate {} of data. Error Message: {}.");
    const Error PMDK_CANT_ALLOCATE_OBJ(207, "Could not add {} of size {} bytes to hashtable. Error Message: {}.");
    const Error PMDK_LOAD_FAILED(208, "Could not load data from PMEM. Error Message: {}.");
    const Error PMDK_FREE_FAILED(209, "Could remove {} from PMEM. Error Message: {}.");
}


#endif //PMEMCPY_ERRORS_H
