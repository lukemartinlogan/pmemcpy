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
#include <pmemcpy/storage/storage.h>
#include <pmemcpy/storage/pmdk_hashtable.h>
#include <pmemcpy/storage/pmdk_fs.h>
#include <pmemcpy/storage/posix.h>
#include <memory>

namespace pmemcpy {

enum class StorageType {
    PMDK_HASHTABLE,
    PMDK_FS,
    POSIX
};

class StorageTypeConverter {
public:
    static StorageType convert(std::string name) {
        if(name == "PMDK_HASHTABLE") return StorageType::PMDK_HASHTABLE;
        if(name == "PMDK_FS") return StorageType::PMDK_FS;
        if(name == "POSIX") return StorageType::POSIX;
        throw INVALID_STORAGE_TYPE.format(name);
    }
    static std::string convert(StorageType id) {
        switch(id) {
            case StorageType::PMDK_HASHTABLE: return "PMDK_HASHTABLE";
            case StorageType::PMDK_FS: return "PMDK_FS";
            case StorageType::POSIX: return "POSIX";
        }
    }
};

class StorageFactory {
public:
    static std::unique_ptr<Storage> get(StorageType type) {
        switch(type) {
            case StorageType::PMDK_HASHTABLE: {
                return std::unique_ptr<pmdk::hash::PMDKHashtableStorage>(new pmdk::hash::PMDKHashtableStorage());
            }
            case StorageType::PMDK_FS: {
                return std::unique_ptr<pmdk::fs::FilesystemPMDKStorage>(new pmdk::fs::FilesystemPMDKStorage());
            }
            case StorageType::POSIX: {
                return std::unique_ptr<PosixStorage>(new PosixStorage());
            }
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_STORAGE_FACTORY_H
