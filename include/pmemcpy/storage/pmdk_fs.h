//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_PMDK_FS_H
#define PMEMCPY_PMDK_FS_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/serializeable.h>
#include <pmemcpy/storage/storage.h>
#include <boost/filesystem.hpp>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libpmem.h>

namespace pmemcpy::pmdk::fs {

struct mmap_buffer : public generic_buffer {
public:
    char *buf_;
    size_t size_;

    mmap_buffer() : buf_(nullptr), size_(0) {}
    mmap_buffer(std::string path, size_t size) { alloc(path, size); }
    mmap_buffer(std::string path) { load(path); }
    mmap_buffer(const mmap_buffer& old) {
        buf_ = old.c_str();
        size_ = old.size();
    }
    mmap_buffer(mmap_buffer&& old) {
        buf_ = old.c_str();
        size_ = old.size();
        old.buf_ = nullptr;
        old.size_ = 0;
    }
    ~mmap_buffer() {
        if(buf_ && buf_ != nullptr) {
            pmem_persist(buf_, size_);
            pmem_unmap(buf_, size_);
        }
    }

    inline void alloc(std::string path, size_t size) {
        size_ = size;
        buf_ = (char*)pmem_map_file(path.c_str(), size, PMEM_FILE_CREATE | PMEM_FILE_SPARSE, 0666, NULL, NULL);
        if(buf_ == nullptr) {
            throw POSIX_MMAP_FAILED.format(SizeType(size, SizeType::MB), path, std::string(strerror(errno)));
        }
    }
    inline void load(std::string path) {
        buf_ = (char*)pmem_map_file(path.c_str(), 0, 0, 0666, &size_, NULL);
        if(buf_ == nullptr) {
            throw POSIX_MMAP_FAILED.format(SizeType(size_, SizeType::MB), path, std::string(strerror(errno)));
        }
    }
    inline size_t size() const { return size_; }
    inline char *c_str() const { return (char*)buf_; }
    char& operator [](int i) { return buf_[i]; }

};

class FilesystemPMDKStorage : public Storage {
private:
    std::string path_prefix_;

public:
    FilesystemPMDKStorage() {}
    ~FilesystemPMDKStorage() {}

    void mmap(std::string path, uint64_t size = 0) {
        AUTO_TRACE("pmemcpy::PosixStorage::mmap {}", path);
        if(!boost::filesystem::exists(path)) {
            boost::filesystem::create_directory(path.c_str());
        }
        if(!boost::filesystem::is_directory(path)) {
            throw POSIX_PATH_NOT_DIRECTORY.format(path);
        }
        path_prefix_ = path;
    }

    void munmap() {
        AUTO_TRACE("pmemcpy::PosixStorage::munmap");
    }

    void release(std::string path) {
        AUTO_TRACE("pmemcpy::PosixStorage::release");
    }

    std::shared_ptr<pmemcpy::generic_buffer> alloc(std::string id, size_t size) {
        AUTO_TRACE("pmemcpy::PosixStorage::alloc id={}, size={}", id, SizeType(size, SizeType::MB));
        std::string path = path_prefix_ + "/" + id;
        std::shared_ptr<pmemcpy::pmdk::fs::mmap_buffer> buf(new pmemcpy::pmdk::fs::mmap_buffer(path, size));
        return buf;
    }

    void store(std::string id, std::shared_ptr<pmemcpy::generic_buffer> &src) {
        AUTO_TRACE("pmemcpy::PosixStorage::store id={}, size={}", id, SizeType(src->size(), SizeType::MB));
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0666);
        if(fd < 0) {
            throw POSIX_OPEN_FAILED.format(fd, path_prefix_, id, std::string(strerror(errno)));
        }
        size_t ret = write(fd, src->c_str(), src->size());
        if(src->size() != ret) {
            close(fd);
            throw POSIX_WRITE_FAILED.format(src->size(), path_prefix_, id, ret, std::string(strerror(errno)));
        }
        close(fd);
    }

    std::shared_ptr<pmemcpy::generic_buffer> find(std::string id) {
        AUTO_TRACE("pmemcpy::PosixStorage::find id={}", id);
        std::string path = path_prefix_ + "/" + id;
        if(!boost::filesystem::exists(path)) {
            throw POSIX_FIND_FAILED.format(path_prefix_, id, std::string(strerror(errno)));
        }
        return std::shared_ptr<pmemcpy::pmdk::fs::mmap_buffer>(new pmemcpy::pmdk::fs::mmap_buffer(path));
    }

    std::shared_ptr<pmemcpy::generic_buffer> load(std::string id) {
        AUTO_TRACE("pmemcpy::PosixStorage::load id={}", id);
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_RDONLY | O_CREAT, 0666);
        if(fd < 0) {
            throw POSIX_READ_FAILED.format(fd, path_prefix_, id, fd, std::string(strerror(errno)));
        }
        size_t len = boost::filesystem::file_size(path);
        char *buffer = (char*)malloc(len);
        size_t ret = read(fd, buffer, len);
        if(len != ret) {
            close(fd);
            throw POSIX_READ_FAILED.format(len, path_prefix_, id, ret, std::string(strerror(errno)));
        }
        close(fd);
        return std::shared_ptr<pmemcpy::malloc_buffer>(new pmemcpy::malloc_buffer(buffer, len));
    }

    void free(std::string id) {
        AUTO_TRACE("pmemcpy::PosixStorage::free id={}", id);
        std::string path = path_prefix_ + "/" + id;
        remove(path.c_str());
    }
};

}

#endif //PMEMCPY_PMDK_FS_H
