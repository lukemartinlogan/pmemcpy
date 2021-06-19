//
// Created by lukemartinlogan on 6/7/21.
//

#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include "pmemulator.h"
#include <dlfcn.h>
#include <pmemcpy/util/trace.h>
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
#include <aio.h>

#define DRAM_BW (68.0*(1ul<<30))
#define DRAM_LATENCY 100
#define PMEM_READ_BW (30.0*(1ul<<30))
#define PMEM_WRITE_BW (7.8*(1ul<<30))
#define PMEM_READ_LATENCY 300
#define PMEM_WRITE_LATENCY 125
inline void nsleep(long delay) {
    struct timespec req;
    req.tv_nsec = delay;
    req.tv_sec = 0;
    nanosleep(&req, NULL);
}
#define READ_PENALTY(size) ((PMEM_READ_LATENCY - DRAM_LATENCY) + ((size)/PMEM_READ_BW - (size)/DRAM_BW))
#define WRITE_PENALTY(size) ((PMEM_WRITE_LATENCY - DRAM_LATENCY) + ((size)/PMEM_WRITE_BW - (size)/DRAM_BW))
#define ADD_READ_PENALTY(size) nsleep(READ_PENALTY(size))
#define ADD_WRITE_PENALTY(size) nsleep(WRITE_PENALTY(size))
inline size_t IOV_COUNT(const struct iovec *iov, int iovcnt) {
    size_t count = 0;
    for(int i = 0; i < iovcnt; ++i) { count += iov->iov_len; }
    return count;
}
#define AIO_COUNT(aiocbp) aiocbp->aio_nbytes




/**
 * PROTOTYPES
 * */

#define WRAPPER_FUN(x) x
#define REAL_FUN(x) __real_##x
#define FNAME_TYPE(T, fname, ...) T (*)(__VA_ARGS__)
#define FORWARD_DECL(T, fname, ...) \
    T (*REAL_FUN(fname))(__VA_ARGS__);
#define GETFUN(T, fname, ...) \
    REAL_FUN(fname) = (FNAME_TYPE(T, fname, __VA_ARGS__))dlsym(RTLD_NEXT, #fname);

FORWARD_DECL(int, open, const char *path, int flags, ...)
FORWARD_DECL(int, __open_2, const char *path, int oflag)
FORWARD_DECL(int, open64, const char *path, int flags, ...)
FORWARD_DECL(int, openat, int dirfd, const char *pathname, int flags, ...)
FORWARD_DECL(int, openat64, int dirfd, const char *pathname, int flags, ...)
FORWARD_DECL(int, creat, const char* path, mode_t mode)
FORWARD_DECL(int, creat64, const char* path, mode_t mode)
FORWARD_DECL(int, dup, int oldfd)
FORWARD_DECL(int, dup2, int oldfd, int newfd)
FORWARD_DECL(int, dup3, int oldfd, int newfd, int flags)
FORWARD_DECL(int, fileno, FILE *stream)
FORWARD_DECL(int, mkstemp, char *temp)
FORWARD_DECL(int, mkostemp, char *temp, int flags)
FORWARD_DECL(int, mkstemps, char *temp, int suffixlen)
FORWARD_DECL(int, mkostemps, char *temp, int suffixlen, int flags)
FORWARD_DECL(ssize_t, read, int fd, void *buf, size_t count)
FORWARD_DECL(ssize_t, write, int fd, const void *buf, size_t count)
FORWARD_DECL(ssize_t, pread, int fd, void *buf, size_t count, off_t offset)
FORWARD_DECL(ssize_t, pwrite, int fd, const void *buf, size_t count, off_t offset)
FORWARD_DECL(ssize_t, pread64, int fd, void *buf, size_t count, off64_t offset)
FORWARD_DECL(ssize_t, pwrite64, int fd, const void *buf, size_t count, off64_t offset)
FORWARD_DECL(ssize_t, readv, int fd, const struct iovec *iov, int iovcnt)
FORWARD_DECL(ssize_t, preadv, int fd, const struct iovec *iov, int iovcnt, off_t offset)
FORWARD_DECL(ssize_t, preadv64, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
FORWARD_DECL(ssize_t, preadv2, int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
FORWARD_DECL(ssize_t, preadv64v2, int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
FORWARD_DECL(ssize_t, writev, int fd, const struct iovec *iov, int iovcnt)
FORWARD_DECL(ssize_t, pwritev, int fd, const struct iovec *iov, int iovcnt, off_t offset)
FORWARD_DECL(ssize_t, pwritev64, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
FORWARD_DECL(ssize_t, pwritev2, int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
FORWARD_DECL(ssize_t, pwritev64v2, int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
FORWARD_DECL(off_t, lseek, int fd, off_t offset, int whence)
FORWARD_DECL(off64_t, lseek64, int fd, off64_t offset, int whence)
FORWARD_DECL(int, __xstat, int vers, const char *path, struct stat *buf)
FORWARD_DECL(int, __xstat64, int vers, const char *path, struct stat64 *buf)
FORWARD_DECL(int, __lxstat, int vers, const char *path, struct stat *buf)
FORWARD_DECL(int, __lxstat64, int vers, const char *path, struct stat64 *buf)
FORWARD_DECL(int, __fxstat, int vers, int fd, struct stat *buf)
FORWARD_DECL(int, __fxstat64, int vers, int fd, struct stat64 *buf)
FORWARD_DECL(void*, mmap, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
FORWARD_DECL(void*, mmap64, void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
FORWARD_DECL(int, fsync, int fd)
FORWARD_DECL(int, fdatasync, int fd)
FORWARD_DECL(int, close, int fd)
FORWARD_DECL(int, aio_read, struct aiocb *aiocbp)
FORWARD_DECL(int, aio_write, struct aiocb *aiocbp)
FORWARD_DECL(int, aio_read64, struct aiocb64 *aiocbp)
FORWARD_DECL(int, aio_write64, struct aiocb64 *aiocbp)
FORWARD_DECL(ssize_t, aio_return, struct aiocb *aiocbp)
FORWARD_DECL(ssize_t, aio_return64, struct aiocb64 *aiocbp)
FORWARD_DECL(int, lio_listio, int mode, struct aiocb *const aiocb_list[], int nitems, struct sigevent *sevp)
FORWARD_DECL(int, lio_listio64, int mode, struct aiocb64 *const aiocb_list[], int nitems, struct sigevent *sevp)
FORWARD_DECL(int, rename, const char *oldpath, const char *newpath)


/**
 * INIT FUNCTIONS
 * */

void init_pmemulator(void) {
    GETFUN(int, open, const char *path, int flags, ...)
    GETFUN(int, __open_2, const char *path, int oflag)
    GETFUN(int, open64, const char *path, int flags, ...)
    GETFUN(int, openat, int dirfd, const char *pathname, int flags, ...)
    GETFUN(int, openat64, int dirfd, const char *pathname, int flags, ...)
    GETFUN(int, creat, const char* path, mode_t mode)
    GETFUN(int, creat64, const char* path, mode_t mode)
    GETFUN(int, dup, int oldfd)
    GETFUN(int, dup2, int oldfd, int newfd)
    GETFUN(int, dup3, int oldfd, int newfd, int flags)
    GETFUN(int, fileno, FILE *stream)
    GETFUN(int, mkstemp, char *temp)
    GETFUN(int, mkostemp, char *temp, int flags)
    GETFUN(int, mkstemps, char *temp, int suffixlen)
    GETFUN(int, mkostemps, char *temp, int suffixlen, int flags)
    GETFUN(ssize_t, read, int fd, void *buf, size_t count)
    GETFUN(ssize_t, write, int fd, const void *buf, size_t count)
    GETFUN(ssize_t, pread, int fd, void *buf, size_t count, off_t offset)
    GETFUN(ssize_t, pwrite, int fd, const void *buf, size_t count, off_t offset)
    GETFUN(ssize_t, pread64, int fd, void *buf, size_t count, off64_t offset)
    GETFUN(ssize_t, pwrite64, int fd, const void *buf, size_t count, off64_t offset)
    GETFUN(ssize_t, readv, int fd, const struct iovec *iov, int iovcnt)
    GETFUN(ssize_t, preadv, int fd, const struct iovec *iov, int iovcnt, off_t offset)
    GETFUN(ssize_t, preadv64, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
    GETFUN(ssize_t, preadv2, int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
    GETFUN(ssize_t, preadv64v2, int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
    GETFUN(ssize_t, writev, int fd, const struct iovec *iov, int iovcnt)
    GETFUN(ssize_t, pwritev, int fd, const struct iovec *iov, int iovcnt, off_t offset)
    GETFUN(ssize_t, pwritev64, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
    GETFUN(ssize_t, pwritev2, int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
    GETFUN(ssize_t, pwritev64v2, int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
    GETFUN(off_t, lseek, int fd, off_t offset, int whence)
    GETFUN(off64_t, lseek64, int fd, off64_t offset, int whence)
    GETFUN(int, __xstat, int vers, const char *path, struct stat *buf)
    GETFUN(int, __xstat64, int vers, const char *path, struct stat64 *buf)
    GETFUN(int, __lxstat, int vers, const char *path, struct stat *buf)
    GETFUN(int, __lxstat64, int vers, const char *path, struct stat64 *buf)
    GETFUN(int, __fxstat, int vers, int fd, struct stat *buf)
    GETFUN(int, __fxstat64, int vers, int fd, struct stat64 *buf)
    GETFUN(void*, mmap, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
    GETFUN(void*, mmap64, void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
    GETFUN(int, fsync, int fd)
    GETFUN(int, fdatasync, int fd)
    GETFUN(int, close, int fd)
    GETFUN(int, aio_read, struct aiocb *aiocbp)
    GETFUN(int, aio_write, struct aiocb *aiocbp)
    GETFUN(int, aio_read64, struct aiocb64 *aiocbp)
    GETFUN(int, aio_write64, struct aiocb64 *aiocbp)
    GETFUN(ssize_t, aio_return, struct aiocb *aiocbp)
    GETFUN(ssize_t, aio_return64, struct aiocb64 *aiocbp)
    GETFUN(int, lio_listio, int mode, struct aiocb *const aiocb_list[], int nitems, struct sigevent *sevp)
    GETFUN(int, lio_listio64, int mode, struct aiocb64 *const aiocb_list[], int nitems, struct sigevent *sevp)
    GETFUN(int, rename, const char *oldpath, const char *newpath)
}

/**
 * WRAPPER FUNCTIONS
 * */


int WRAPPER_FUN(open)(const char *path, int flags, ...)
{
    AUTO_TRACE("open: path={}, flags={}", path, flags);
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
    AUTO_TRACE("__open_2: path={}, flags={}", path, oflag);
    return REAL_FUN(__open_2)(path, oflag);
}

int WRAPPER_FUN(open64)(const char *path, int flags, ...)
{
    AUTO_TRACE("open64: path={}, flags={}", path, flags);
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
    AUTO_TRACE("openat: path={}, flags={}", pathname, flags);
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
    AUTO_TRACE("openat64: dirfd={}, pathname={}, flags={}", dirfd, pathname, flags);
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
    AUTO_TRACE("creat: path={}, mode={}", path, mode);
    return REAL_FUN(creat)(path, mode);
}

int WRAPPER_FUN(creat64)(const char* path, mode_t mode)
{
    AUTO_TRACE("creat64: path={}, mode={}", path, mode);
    return REAL_FUN(creat64)(path, mode);
}

int WRAPPER_FUN(dup)(int oldfd)
{
    AUTO_TRACE("dup: oldfd={}", oldfd);
    return REAL_FUN(dup)(oldfd);
}

int WRAPPER_FUN(dup2)(int oldfd, int newfd)
{
    AUTO_TRACE("dup: oldfd={}, newfd={}", oldfd, newfd);
    return REAL_FUN(dup2)(oldfd, newfd);
}

int WRAPPER_FUN(dup3)(int oldfd, int newfd, int flags)
{
    AUTO_TRACE("dup3: oldfd={}, newfd={}, flags={}", oldfd, newfd, flags);
    return REAL_FUN(dup3)(oldfd, newfd, flags);
}

int WRAPPER_FUN(fileno)(FILE *stream)
{
    AUTO_TRACE("fileno");
    return REAL_FUN(fileno)(stream);
}

int WRAPPER_FUN(mkstemp)(char *temp)
{
    AUTO_TRACE("mkstemp: template={}", temp);
    return REAL_FUN(mkstemp)(temp);
}

int WRAPPER_FUN(mkostemp)(char *temp, int flags)
{
    AUTO_TRACE("mkostemp: template={}, flags={}", temp, flags);
    return REAL_FUN(mkostemp)(temp, flags);
}

int WRAPPER_FUN(mkstemps)(char *temp, int suffixlen)
{
    AUTO_TRACE("mkstemps: template={}, suffixlen={}", temp, suffixlen);
    return REAL_FUN(mkstemps)(temp, suffixlen);
}

int WRAPPER_FUN(mkostemps)(char *temp, int suffixlen, int flags)
{
    AUTO_TRACE("mkstemps: template={}, suffixlen={}, flags={}", temp, suffixlen, flags);
    return REAL_FUN(mkostemps)(temp, suffixlen, flags);
}

/**
 * I/O FUNCTIONS
 * */

ssize_t WRAPPER_FUN(read)(int fd, void *buf, size_t count)
{
    AUTO_TRACE("read: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), READ_PENALTY(count));
    ADD_READ_PENALTY(count);
    return REAL_FUN(read)(fd, buf, count);
}

ssize_t WRAPPER_FUN(write)(int fd, const void *buf, size_t count)
{
    AUTO_TRACE("write: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), WRITE_PENALTY(count));
    ADD_WRITE_PENALTY(count);
    return REAL_FUN(write)(fd, buf, count);
}

ssize_t WRAPPER_FUN(pread)(int fd, void *buf, size_t count, off_t offset)
{
    AUTO_TRACE("pread: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), READ_PENALTY(count));
    ADD_READ_PENALTY(count);
    return REAL_FUN(pread)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pwrite)(int fd, const void *buf, size_t count, off_t offset)
{
    AUTO_TRACE("pwrite: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), WRITE_PENALTY(count));
    ADD_WRITE_PENALTY(count);
    return REAL_FUN(pwrite)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pread64)(int fd, void *buf, size_t count, off64_t offset)
{
    AUTO_TRACE("pread64: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), READ_PENALTY(count));
    ADD_READ_PENALTY(count);
    return REAL_FUN(pread64)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(pwrite64)(int fd, const void *buf, size_t count, off64_t offset)
{
    AUTO_TRACE("pwrite64: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(count, pmemcpy::SizeType::MB), WRITE_PENALTY(count));
    ADD_WRITE_PENALTY(count);
    return REAL_FUN(pwrite64)(fd, buf, count, offset);
}

ssize_t WRAPPER_FUN(readv)(int fd, const struct iovec *iov, int iovcnt)
{
    AUTO_TRACE("readv: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), READ_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_READ_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(readv)(fd, iov, iovcnt);
}

ssize_t WRAPPER_FUN(preadv)(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    AUTO_TRACE("preadv: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), READ_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_READ_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(preadv)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(preadv64)(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    AUTO_TRACE("preadv64: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), READ_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_READ_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(preadv64)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(preadv2)(int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
{
    AUTO_TRACE("preadv2: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), READ_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_READ_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(preadv2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(preadv64v2)(int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
{
    AUTO_TRACE("preadv64v2: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), READ_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_READ_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(preadv64v2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(writev)(int fd, const struct iovec *iov, int iovcnt)
{
    AUTO_TRACE("writev: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), WRITE_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_WRITE_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(writev)(fd, iov, iovcnt);
}

ssize_t WRAPPER_FUN(pwritev)(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    AUTO_TRACE("pwritev: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), WRITE_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_WRITE_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(pwritev)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(pwritev64)(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    AUTO_TRACE("pwritev64: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), WRITE_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_WRITE_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(pwritev64)(fd, iov, iovcnt, offset);
}

ssize_t WRAPPER_FUN(pwritev2)(int fd, const struct iovec *iov, int iovcnt, off_t offset, int flags)
{
    AUTO_TRACE("pwritev2: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), WRITE_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_WRITE_PENALTY(IOV_COUNT(iov, iovcnt));
    return REAL_FUN(pwritev2)(fd, iov, iovcnt, offset, flags);
}

ssize_t WRAPPER_FUN(pwritev64v2)(int fd, const struct iovec *iov, int iovcnt, off64_t offset, int flags)
{
    AUTO_TRACE("pwritev64v2: fd={}, size={}, penalty={}ns", fd, pmemcpy::SizeType(IOV_COUNT(iov, iovcnt), pmemcpy::SizeType::MB), WRITE_PENALTY(IOV_COUNT(iov, iovcnt)));
    ADD_WRITE_PENALTY(IOV_COUNT(iov, iovcnt));
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

void* WRAPPER_FUN(mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    return REAL_FUN(mmap)(addr, length, prot, flags, fd, offset);
}

void* WRAPPER_FUN(mmap64)(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
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
    AUTO_TRACE("aio_read: size={}, penalty={}ns", pmemcpy::SizeType(AIO_COUNT(aiocbp), pmemcpy::SizeType::MB), READ_PENALTY(AIO_COUNT(aiocbp)));
    ADD_READ_PENALTY(AIO_COUNT(aiocbp));
    return REAL_FUN(aio_read)(aiocbp);
}

int WRAPPER_FUN(aio_write)(struct aiocb *aiocbp)
{
    AUTO_TRACE("aio_write: size={}, penalty={}ns", pmemcpy::SizeType(AIO_COUNT(aiocbp), pmemcpy::SizeType::MB), WRITE_PENALTY(AIO_COUNT(aiocbp)));
    ADD_WRITE_PENALTY(AIO_COUNT(aiocbp));
    return REAL_FUN(aio_write)(aiocbp);
}

int WRAPPER_FUN(aio_read64)(struct aiocb64 *aiocbp)
{
    AUTO_TRACE("aio_read64: size={}, penalty={}ns", pmemcpy::SizeType(AIO_COUNT(aiocbp), pmemcpy::SizeType::MB), READ_PENALTY(AIO_COUNT(aiocbp)));
    ADD_READ_PENALTY(AIO_COUNT(aiocbp));
    return REAL_FUN(aio_read64)(aiocbp);
}

int WRAPPER_FUN(aio_write64)(struct aiocb64 *aiocbp)
{
    AUTO_TRACE("aio_write64: size={}, penalty={}ns", pmemcpy::SizeType(AIO_COUNT(aiocbp), pmemcpy::SizeType::MB), WRITE_PENALTY(AIO_COUNT(aiocbp)));
    ADD_WRITE_PENALTY(AIO_COUNT(aiocbp));
    return REAL_FUN(aio_write64)(aiocbp);
}

ssize_t WRAPPER_FUN(aio_return)(struct aiocb *aiocbp)
{
    AUTO_TRACE("aio_return");
    return REAL_FUN(aio_return)(aiocbp);
}

ssize_t WRAPPER_FUN(aio_return64)(struct aiocb64 *aiocbp)
{
    AUTO_TRACE("aio_return64");
    return REAL_FUN(aio_return64)(aiocbp);
}

int WRAPPER_FUN(lio_listio)(int mode, struct aiocb *const aiocb_list[], int nitems, struct sigevent *sevp)
{
    AUTO_TRACE("lio_listio");
    printf("This actually got used???\n");
    return REAL_FUN(lio_listio)(mode, aiocb_list, nitems, sevp);
}

int WRAPPER_FUN(lio_listio64)(int mode, struct aiocb64 *const aiocb_list[], int nitems, struct sigevent *sevp)
{
    AUTO_TRACE("lio_listio64");
    return REAL_FUN(lio_listio64)(mode, aiocb_list, nitems, sevp);
}

int WRAPPER_FUN(rename)(const char *oldpath, const char *newpath)
{
    return REAL_FUN(rename)(oldpath, newpath);
}