//
// Created by lukemartinlogan on 5/28/21.
//

/**
 * This file loads/stores serialized data into PMEM
 * It implements data layout policies
 * */

#ifndef PMEMCPY_STORAGE_H
#define PMEMCPY_STORAGE_H

#include <string>

namespace pmemcpy {

class Storage {
public:
    virtual void mmap(std::string path, uint64_t size = 0) = 0;
    virtual void munmap() = 0;
    virtual void release(std::string path) = 0;
    virtual void store(std::string id, std::string &src) = 0;
    virtual std::string load(std::string id) = 0;
    virtual void free(std::string id) = 0;
};

}

#endif //PMEMCPY_STORAGE_H
