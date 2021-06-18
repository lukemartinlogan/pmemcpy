//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_POSIX_H
#define PMEMCPY_POSIX_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/storage/storage.h>
#include <boost/filesystem.hpp>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

namespace pmemcpy {

class PosixStorage : public Storage {
private:
    std::string path_prefix_;

public:
    PosixStorage() {}
    ~PosixStorage() {}

    void mmap(std::string path, uint64_t size = 0) {
        if(!boost::filesystem::is_directory(path)) {
            throw POSIX_PATH_NOT_DIRECTORY.format(path);
        }
        path_prefix_ = path;
    }

    void munmap() {}

    void release(std::string path) {}

    void store(std::string id, std::string &src) {
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0666);
        if(fd < 0) {
            throw POSIX_WRITE_FAILED.format(fd, path_prefix_, id, fd, std::string(strerror(errno)));
        }
        size_t ret = write(fd, src.c_str(), src.size());
        if(src.size() != ret) {
            throw POSIX_WRITE_FAILED.format(src.size(), path_prefix_, id, ret, std::string(strerror(errno)));
        }
    }

    std::string load(std::string id) {
        std::string path = path_prefix_ + "/" + id;
        int fd = open(path.c_str(), O_RDONLY | O_CREAT, 0666);
        if(fd < 0) {
            throw POSIX_READ_FAILED.format(fd, path_prefix_, id, fd, std::string(strerror(errno)));
        }
        size_t len = boost::filesystem::file_size(path);
        char *buffer = (char*)malloc(len);
        size_t ret = read(fd, buffer, len);
        if(len != ret) {
            throw POSIX_READ_FAILED.format(len, path_prefix_, id, ret, std::string(strerror(errno)));
        }
        return std::string(buffer, len);
    }

    void free(std::string id) {
        std::string path = path_prefix_ + "/" + id;
        remove(path.c_str());
    }
};

}

#endif //PMEMCPY_POSIX_H
