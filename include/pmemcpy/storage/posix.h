//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_POSIX_H
#define PMEMCPY_POSIX_H

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

namespace pmemcpy {

struct mmap_buffer : public generic_buffer {
public:
    char *buf_;
    size_t size_;
    int fd_;

    mmap_buffer() : buf_(nullptr), size_(0), fd_(-1) {}
    mmap_buffer(int fd, size_t size) { alloc(fd, size); }
    mmap_buffer(int fd) { load(fd); }
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
        if(buf_ && buf_ != MAP_FAILED) {
            _msync();
            munmap(buf_, size_);
        }
    }
    void _msync() {
        if(buf_ && buf_ != MAP_FAILED) {
            int ret = msync(buf_, size_, MS_SYNC);
            if (ret < 0) {
                printf("Yep, it failed..\n");
                exit(1);
            }
        }
    }

    inline void alloc(int fd, size_t size) {
        size_ = size;
        buf_ = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
        if(buf_ == MAP_FAILED) {
            throw POSIX_MMAP_FAILED.format(SizeType(size, SizeType::MB), fd, std::string(strerror(errno)));
        }
        //_msync();
        fd_ = fd;
        //fsync(fd_);
    }
    inline void load(int fd) {
        size_ = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        buf_ = (char*)mmap(NULL, size_, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
        //_msync();
        fd_ = fd;
        //fsync(fd_);
    }
    inline size_t size() const { return size_; }
    inline char *c_str() const { return (char*)buf_; }
    char& operator [](int i) { return buf_[i]; }

};

class PosixStorage : public Storage {
private:
    std::string path_prefix_;

public:
    PosixStorage() {}
    ~PosixStorage() {}

    void mmap(std::string path, uint64_t size = 0) {
        if(!boost::filesystem::exists(path)) {
            mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if(!boost::filesystem::is_directory(path)) {
            throw POSIX_PATH_NOT_DIRECTORY.format(path);
        }
        path_prefix_ = path;
    }

    void munmap() {}

    void release(std::string path) {}

    std::shared_ptr<pmemcpy::generic_buffer> alloc(std::string id, size_t size) {
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
        if(fd < 0) {
            throw POSIX_WRITE_FAILED.format(fd, path_prefix_, id, fd, std::string(strerror(errno)));
        }
        if(ftruncate(fd, size) < 0) {
            throw POSIX_TRUNCATE_FAILED.format(size, path_prefix_, id, std::string(strerror(errno)));
        }
        std::shared_ptr<pmemcpy::mmap_buffer> buf(new pmemcpy::mmap_buffer(fd, size));
        close(fd);
        return buf;
    }

    void store(std::string id, std::shared_ptr<pmemcpy::generic_buffer> &src) {
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
        fsync(fd);
        close(fd);
    }

    std::shared_ptr<pmemcpy::generic_buffer> find(std::string id) {
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_RDWR, 0666);
        if(fd < 0) {
            throw POSIX_FIND_FAILED.format(path_prefix_, id, std::string(strerror(errno)));
        }
        std::shared_ptr<pmemcpy::mmap_buffer> buf(new pmemcpy::mmap_buffer(fd));
        close(fd);
        return buf;
    }

    std::shared_ptr<pmemcpy::generic_buffer> load(std::string id) {
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
        std::string path = path_prefix_ + "/" + id;
        remove(path.c_str());
    }
};

}

#endif //PMEMCPY_POSIX_H
