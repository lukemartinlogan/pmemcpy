//
// Created by lukemartinlogan on 6/7/21.
//


/*
 * Copyright (C) 2015 University of Chicago.
 * See COPYRIGHT notice in top-level directory.
 *
 */

#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include "darshan-runtime-config.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>


#define WRAPPER_FUN(x) __wrap_##x
#define REAL_FUN(x) __real_##x

void io_thread() {
    std::queue<>
}

void init_io_threads() {

}

int WRAPPER_FUN(memcpy)() {

}

int WRAPPER_FUN(open)(const char *path, int flags, ...)
{
    int mode = 0;
    int ret;
    if(flags & O_CREAT) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);
        ret = REAL_FUN(open)(path, flags, mode);
    }
    else {
        ret = REAL_FUN(open)(path, flags);
    }
    return ret;
}

int WRAPPER_FUN(__open_2)(const char *path, int oflag)
{
    return REAL_FUN(__open_2)(path, oflag);
}

int WRAPPER_FUN(open64)(const char *path, int flags, ...)
{
    int mode = 0;
    int ret;
    if(flags & O_CREAT) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);
        ret = REAL_FUN(open64)(path, flags, mode);
    }
    else {
        ret = REAL_FUN(open64)(path, flags);
    }
    return ret;
}

int WRAPPER_FUN(openat)(int dirfd, const char *pathname, int flags, ...)
{
    int mode = 0;
    int ret;
    if(flags & O_CREAT) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);
        ret = REAL_FUN(openat)(dirfd, pathname, flags, mode);
    }
    else {
        ret = REAL_FUN(openat)(dirfd, pathname, flags);
    }
    return ret;
}

int WRAPPER_FUN(openat64)(int dirfd, const char *pathname, int flags, ...)
{
    int mode = 0;
    int ret;
    if(flags & O_CREAT) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);
        ret = REAL_FUN(openat64)(dirfd, pathname, flags, mode);
    }
    else {
        ret = REAL_FUN(openat64)(dirfd, pathname, flags);
    }
    return ret;
}

int WRAPPER_FUN(creat)(const char* path, mode_t mode)
{
    return REAL_FUN(creat)(path, mode);
}

int WRAPPER_FUN(creat64)(const char* path, mode_t mode)
{
    return REAL_FUN(creat64)(path, mode);
}

int WRAPPER_FUN(dup)(int oldfd)
{
    return REAL_FUN(dup)(oldfd);
}

int WRAPPER_FUN(dup2)(int oldfd, int newfd)
{
    return REAL_FUN(dup2)(oldfd, newfd);
}

int WRAPPER_FUN(dup3)(int oldfd, int newfd, int flags)
{
    return REAL_FUN(dup3)(oldfd, newfd, flags);
}

int WRAPPER_FUN(fileno)(FILE *stream)
{
    return REAL_FUN(fileno)(stream);
}

int WRAPPER_FUN(mkstemp)(char* template)
{
    return REAL_FUN(mkstemp)(template);
}

int WRAPPER_FUN(mkostemp)(char* template, int flags)
{
    return REAL_FUN(mkostemp)(template, flags);
}

int WRAPPER_FUN(mkstemps)(char* template, int suffixlen)
{
    return REAL_FUN(mkstemps)(template, suffixlen);
}

int WRAPPER_FUN(mkostemps)(char* template, int suffixlen, int flags)
{
    return REAL_FUN(mkostemps)(template, suffixlen, flags);
}

ssize_t WRAPPER_FUN(read)(int fd, void *buf, size_t count)
{
    return REAL_FUN(read)(fd, buf, count);
}

ssize_t WRAPPER_FUN(write)(int fd, const void *buf, size_t count)
{
    return REAL_FUN(write)(fd, buf, count);
}

ssize_t WRAPPER_FUN(pread)(int fd, void *buf, size_t count, off_t offset)
{
    return REAL_FUN(pread)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pwrite)(int fd, const void *buf, size_t count, off_t offset)
{
    return REAL_FUN(pwrite)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pread64)(int fd, void *buf, size_t count, off64_t offset)
{
    return REAL_FUN(pread64)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pwrite64)(int fd, const void *buf, size_t count, off64_t offset)
{
    return REAL_FUN(pwrite64)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(readv)(int fd, const struct iovec *iov, int iovcnt)
{
    return REAL_FUN(readv)(fd, iov, iovcnt);
}

ssize_t WRAPPER_FUN(preadv)(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    return REAL_FUN(preadv)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(preadv64)(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    return REAL_FUN(preadv64)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(preadv2)(int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
{
    return REAL_FUN(preadv2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(preadv64v2)(int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
{
    return REAL_FUN(preadv64v2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(writev)(int fd, const struct iovec *iov, int iovcnt)
{
    return REAL_FUN(writev)(fd, iov, iovcnt);
}

ssize_t WRAPPER_FUN(pwritev)(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    return REAL_FUN(pwritev)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(pwritev64)(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    return REAL_FUN(pwritev64)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(pwritev2)(int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
{
    return REAL_FUN(pwritev2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(pwritev64v2)(int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
{
    return REAL_FUN(pwritev64v2)(fd, iov, iovcnt, offset, flags);
}

off_t WRAPPER_FUN(lseek)(int fd, off_t offset, int whence)
{
    return REAL_FUN(lseek)(fd, offset, whence);
}

off64_t WRAPPER_FUN(lseek64)(int fd, off64_t offset, int whence)
{
    return REAL_FUN(lseek64)(fd, offset, whence);
}

int WRAPPER_FUN(__xstat)(int vers, const char *path, struct stat *buf)
{
    return REAL_FUN(__xstat)(vers, path, buf);
}

int WRAPPER_FUN(__xstat64)(int vers, const char *path, struct stat64 *buf)
{
    return REAL_FUN(__xstat64)(vers, path, buf);
}

int WRAPPER_FUN(__lxstat)(int vers, const char *path, struct stat *buf)
{
    return REAL_FUN(__lxstat)(vers, path, buf);
}

int WRAPPER_FUN(__lxstat64)(int vers, const char *path, struct stat64 *buf)
{
    return REAL_FUN(__lxstat64)(vers, path, buf);
}

int WRAPPER_FUN(__fxstat)(int vers, int fd, struct stat *buf)
{
    return REAL_FUN(__fxstat)(vers, fd, buf);
}

int WRAPPER_FUN(__fxstat64)(int vers, int fd, struct stat64 *buf)
{
    return REAL_FUN(__fxstat64)(vers, fd, buf);
}

void* WRAPPER_FUN(mmap)(void *addr, size_t length, int prot, int flags,
    int fd, off_t offset)
{
    return REAL_FUN(mmap)(addr, length, prot, flags, fd, offset);
}

void* WRAPPER_FUN(mmap64)(void *addr, size_t length, int prot, int flags,
    int fd, off64_t offset)
{
    return REAL_FUN(mmap)(addr, length, prot, flags, fd, offset);
}

int WRAPPER_FUN(fsync)(int fd)
{
    return REAL_FUN(fsync)(fd);
}

int WRAPPER_FUN(fdatasync)(int fd)
{
    return REAL_FUN(fdatasync)(fd);
}

int WRAPPER_FUN(close)(int fd)
{
    return REAL_FUN(close)(fd);
}

int WRAPPER_FUN(aio_read)(struct aiocb *aiocbp)
{
    return REAL_FUN(aio_read)(aiocbp);
}

int WRAPPER_FUN(aio_write)(struct aiocb *aiocbp)
{
    return REAL_FUN(aio_write)(aiocbp);
}

int WRAPPER_FUN(aio_read64)(struct aiocb64 *aiocbp)
{
    return REAL_FUN(aio_read64)(aiocbp);
}

int WRAPPER_FUN(aio_write64)(struct aiocb64 *aiocbp)
{
    return REAL_FUN(aio_write64)(aiocbp);
}

ssize_t WRAPPER_FUN(aio_return)(struct aiocb *aiocbp)
{
    return REAL_FUN(aio_return)(aiocbp);
}

ssize_t WRAPPER_FUN(aio_return64)(struct aiocb64 *aiocbp)
{
    return REAL_FUN(aio_return64)(aiocbp);
}

int WRAPPER_FUN(lio_listio)(int mode, struct aiocb *const aiocb_list[],
                             int nitems, struct sigevent *sevp)
{
    return REAL_FUN(lio_listio)(mode, aiocb_list, nitems, sevp);
}

int WRAPPER_FUN(lio_listio64)(int mode, struct aiocb64 *const aiocb_list[],
                               int nitems, struct sigevent *sevp)
{
    return REAL_FUN(lio_listio)(mode, aiocb_list, nitems, sevp);
}

int WRAPPER_FUN(rename)(const char *oldpath, const char *newpath)
{
    return REAL_FUN(rename)(oldpath, newpath);
}
