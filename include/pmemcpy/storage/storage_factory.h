//
// Created by lukemartinlogan on 5/28/21.
//

/**
 * This file is responsible for loading/storing serialized data.
 * It is meant to be called by higher-level APIs.
 * These files implement data layout policies.
 * */

#ifndef PMEMCPY_STORAGE_FACTORY_H
#define PMEMCPY_STORAGE_FACTORY_H

#include <pmemcpy/storage/pmdk_hashtable.h>
#include <pmemcpy/storage/pmdk_list.h>
#include <pmemcpy/storage/posix.h>
#include <memory>

namespace pmemcpy {

enum class StorageType {
    PMDK_LIST,
    PMDK_HASHTABLE,
    POSIX,
    MMAP
};

class StorageFactory {
public:
    static std::shared_ptr<Storage> get(StorageType type) {
        switch(type) {
            case StorageType::PMDK_LIST: {
                return std::make_shared<pmdk::list::PMDKListStorage>();
            }
            case StorageType::PMDK_HASHTABLE: {
                return std::make_shared<pmdk::hash::PMDKHashtableStorage>();
            }
            case StorageType::POSIX: {
                return std::make_shared<PosixStorage>();
            }
            case StorageType::MMAP: {
                return nullptr;
            }
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_STORAGE_FACTORY_H
