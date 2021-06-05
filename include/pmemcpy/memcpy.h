//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_H
#define PMEMCPY_H

#include <pmemcpy/serialize/serializer_factory.h>
#include <pmemcpy/storage/storage_factory.h>
#include <string>
#include <memory>

namespace pmemcpy {

class PMEM {
private:
    SerializerType serializer_;
    std::shared_ptr<Storage> storage_;
public:
    PMEM(StorageType storage=StorageType::PMDK_LIST, SerializerType serializer=SerializerType::MSGPACK) : serializer_(serializer) {
        storage_ = StorageFactory::get(storage);
    }

    inline void mmap(std::string path, uint64_t size = PMEMOBJ_MIN_POOL) {
        storage_->mmap(path, size);
    }
    inline void munmap() {
        storage_->munmap();
    }
    inline void release(std::string path) {
        storage_->release(path);
    }
    template<typename T>
    inline void store(std::string id, T *src, unsigned count = 1) {
        std::string serial = SerializerFactory<T>::get(serializer_)->serialize(src);
        storage_->store(id, serial);
    }
    template<typename T>
    inline void load(std::string id, T *dst) {
        std::string serial = storage_->load(id);
        SerializerFactory<T>::get(serializer_)->deserialize(dst, serial);
    }
    inline void free(std::string id) {
        storage_->free(id);
    }
};

}

#endif //PMEMCPY_H
