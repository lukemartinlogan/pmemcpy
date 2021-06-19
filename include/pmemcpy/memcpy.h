//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_H
#define PMEMCPY_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/trace.h>
#include <pmemcpy/serialize/serializer_factory.h>
#include <pmemcpy/storage/storage_factory.h>
#include <string>
#include <memory>

#ifdef PMEMULATION
#define DRAM_BW 68.0
#define DRAM_LATENCY 100
#define PMEM_READ_BW 30
#define PMEM_WRITE_BW 7.8
#define PMEM_READ_LATENCY 300
#define PMEM_WRITE_LATENCY 125
inline void nsleep(long delay) {
    struct timespec req;
    req.tv_nsec = delay;
    nanosleep(&req, NULL);
}
#define ADD_READ_PENALTY(size) nsleep((PMEM_READ_LATENCY - DRAM_LATENCY) + (size/PMEM_READ_BW - size/DRAM_BW))
#define ADD_WRITE_PENALTY(size) nsleep((PMEM_WRITE_LATENCY - DRAM_LATENCY) + (size/PMEM_WRITE_BW - size/DRAM_BW))
#else
#define ADD_READ_PENALTY(size)
#define ADD_WRITE_PENALTY(size)
#endif

namespace pmemcpy {

class PMEM {
private:
    SerializerType serializer_;
    std::shared_ptr<Storage> storage_;
public:
    PMEM(StorageType storage=StorageType::PMDK_LIST, SerializerType serializer=SerializerType::MSGPACK) : serializer_(serializer) {
        storage_ = StorageFactory::get(storage);
    }

    inline void mmap(std::string path, uint64_t size=0) {
        storage_->mmap(path, size);
    }
    inline void munmap() {
        storage_->munmap();
    }
    inline void release(std::string path) {
        storage_->release(path);
    }
    template<typename T>
    inline void store(std::string id, T &src) {
        std::string serial = SerializerFactory<T>::get(serializer_)->serialize(src);
        ADD_WRITE_PENALTY(serial.size());
        storage_->store(id, serial);
    }
    template<typename T>
    inline void store(std::string id, T *src, Dimensions dims) {
        std::string serial = SerializerFactory<T>::get(serializer_)->serialize(src, dims);
        ADD_WRITE_PENALTY(serial.size());
        storage_->store(id, serial);
    }
    template<typename T>
    inline void store(std::string id, T *src, Offsets offsets, Sizes sizes, Dimensions dims) {
        store(id, src, dims);
    }
    template<typename T>
    inline void load(std::string id, T &dst) {
        std::string serial = storage_->load(id);
        ADD_READ_PENALTY(serial.size());
        SerializerFactory<T>::get(serializer_)->deserialize(dst, serial);
    }
    template<typename T>
    inline void load(std::string id, T *dst, Dimensions dims) {
        std::string serial = storage_->load(id);
        ADD_READ_PENALTY(serial.size());
        SerializerFactory<T>::get(serializer_)->deserialize(dst, serial, dims);
    }
    template<typename T>
    inline void load(std::string id, T *dst, Offsets offsets, Sizes sizes, Dimensions dims) {
        load(id, dst, dims);
    }
    inline void free(std::string id) {
        storage_->free(id);
    }
};

}

#endif //PMEMCPY_H
