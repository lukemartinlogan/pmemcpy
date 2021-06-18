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

#include <pmemcpy/util/errors.h>
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

class StorageTypeConverter {
public:
    static StorageType convert(std::string name) {
        if(name == "PMDK_LIST") return StorageType::PMDK_LIST;
        if(name == "PMDK_HASHTABLE") return StorageType::PMDK_HASHTABLE;
        if(name == "POSIX") return StorageType::POSIX;
        if(name == "MMAP") return StorageType::MMAP;
        throw INVALID_STORAGE_TYPE.format(name);
    }
    static std::string convert(StorageType id) {
        switch(id) {
            case StorageType::PMDK_LIST: return "PMDK_LIST";
            case StorageType::PMDK_HASHTABLE: return "PMDK_HASHTABLE";
            case StorageType::POSIX: return "POSIX";
            case StorageType::MMAP: return "MMAP";
        }
    }
};

class StorageFactory {
public:
    static std::shared_ptr<Storage> get(StorageType type) {
        switch(type) {
            case StorageType::PMDK_LIST: {
                return std::shared_ptr<pmdk::list::PMDKListStorage>(new pmdk::list::PMDKListStorage());
            }
            case StorageType::PMDK_HASHTABLE: {
                return std::shared_ptr<pmdk::hash::PMDKHashtableStorage>(new pmdk::hash::PMDKHashtableStorage());
            }
            case StorageType::POSIX: {
                return std::shared_ptr<PosixStorage>(new PosixStorage());
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
