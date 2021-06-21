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
#include <pmemcpy/storage/posix.h>
#include <memory>

namespace pmemcpy {

enum class StorageType {
    PMDK_HASHTABLE,
    POSIX
};

class StorageTypeConverter {
public:
    static StorageType convert(std::string name) {
        if(name == "PMDK_HASHTABLE") return StorageType::PMDK_HASHTABLE;
        if(name == "POSIX") return StorageType::POSIX;
        throw INVALID_STORAGE_TYPE.format(name);
    }
    static std::string convert(StorageType id) {
        switch(id) {
            case StorageType::PMDK_HASHTABLE: return "PMDK_HASHTABLE";
            case StorageType::POSIX: return "POSIX";
        }
    }
};

class StorageFactory {
public:
    static std::shared_ptr<Storage> get(StorageType type) {
        switch(type) {
            case StorageType::PMDK_HASHTABLE: {
                return std::shared_ptr<pmdk::hash::PMDKHashtableStorage>(new pmdk::hash::PMDKHashtableStorage());
            }
            case StorageType::POSIX: {
                return std::shared_ptr<PosixStorage>(new PosixStorage());
            }
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_STORAGE_FACTORY_H
