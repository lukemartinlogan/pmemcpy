//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_H
#define PMEMCPY_H

#define BOOST_USE_VALGRIND

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/trace.h>
#include <pmemcpy/util/vector_ptr.h>
#include <pmemcpy/serialize/serializer_factory.h>
#include <pmemcpy/storage/storage_factory.h>
#include <string>
#include <memory>

#ifdef PMEMULATION
#define DRAM_BW 68
#define DRAM_LATENCY 100
#define PMEM_READ_BW 30
#define PMEM_WRITE_BW 8
#define PMEM_READ_LATENCY 300
#define PMEM_WRITE_LATENCY 125
inline void nsleep(long delay) {
    struct timespec req;
    req.tv_nsec = delay;
    nanosleep(&req, NULL);
}
#define READ_PENALTY(size) (long)((PMEM_READ_LATENCY - DRAM_LATENCY) + ((size)/PMEM_READ_BW - (size)/DRAM_BW))
#define WRITE_PENALTY(size) (long)((PMEM_WRITE_LATENCY - DRAM_LATENCY) + ((size)/PMEM_WRITE_BW - (size)/DRAM_BW))
#define ADD_READ_PENALTY(size) nsleep(READ_PENALTY(size))
#define ADD_WRITE_PENALTY(size) nsleep(WRITE_PENALTY(size))
#else
#define ADD_READ_PENALTY(size)
#define ADD_WRITE_PENALTY(size)
#endif

namespace pmemcpy {

class PMEM {
private:
    SerializerType serializer_;
    std::shared_ptr<Storage> storage_;
    bool use_mmap_;
public:
    PMEM(StorageType storage=StorageType::PMDK_HASHTABLE, SerializerType serializer=SerializerType::CAPNPROTO, bool use_mmap=false) : serializer_(serializer), use_mmap_(use_mmap) {
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
        if(use_mmap_) {
            auto serial = SerializerFactory<T>::get(serializer_);
            std::shared_ptr<pmemcpy::generic_buffer> buf = storage_->alloc(id, serial->est_encoded_size(sizeof(T)));
            size_t size = SerializerFactory<T>::get(serializer_)->serialize(buf, src);
            ADD_WRITE_PENALTY(size);
        } else {
            std::shared_ptr<pmemcpy::generic_buffer> serial = SerializerFactory<T>::get(serializer_)->serialize(src);
            storage_->store(id, serial);
        }
    }
    template<typename T>
    inline void store(std::string id, T *src, Dimensions dims) {
        if(use_mmap_) {
            auto serial = SerializerFactory<T>::get(serializer_);
            std::shared_ptr<pmemcpy::generic_buffer> buf = storage_->alloc(id, serial->est_encoded_size(sizeof(T)*dims.count()));
            size_t size = serial->serialize(buf, src, dims);
            ADD_WRITE_PENALTY(size);
        } else {
            std::shared_ptr<pmemcpy::generic_buffer> serial = SerializerFactory<T>::get(serializer_)->serialize(src, dims);
            storage_->store(id, serial);
        }
    }
    template<typename T>
    inline void store(std::string id, T *src, Offsets offsets, Sizes sizes, Dimensions dims) {
        store(id, src, dims);
    }
    template<typename T>
    inline void load(std::string id, T &dst) {
        if(use_mmap_) {
            std::shared_ptr<pmemcpy::generic_buffer> serial = storage_->find(id);
            SerializerFactory<T>::get(serializer_)->deserialize(dst, serial);
            ADD_READ_PENALTY(serial->size());
        } else {
            std::shared_ptr<pmemcpy::generic_buffer> serial = storage_->load(id);
            SerializerFactory<T>::get(serializer_)->deserialize(dst, serial);
        }
    }
    template<typename T>
    inline void load(std::string id, T *dst, Dimensions dims) {
        if(use_mmap_) {
            std::shared_ptr<pmemcpy::generic_buffer> serial = storage_->find(id);
            SerializerFactory<T>::get(serializer_)->deserialize(dst, serial, dims);
            ADD_READ_PENALTY(serial->size());
        } else {
            std::shared_ptr<pmemcpy::generic_buffer> serial = storage_->load(id);
            SerializerFactory<T>::get(serializer_)->deserialize(dst, serial, dims);
        }
    }
    template<typename T>
    inline void load(std::string id, T *dst, Offsets offsets, Sizes sizes, Dimensions dims) {
        //TODO: Actually use offset information when loading data
        load(id, dst, dims);
    }
    inline void free(std::string id) {
        storage_->free(id);
    }
};

}

#endif //PMEMCPY_H
