//
// Created by lukemartinlogan on 6/9/21.
//

#include <pmemulator.h>
#include <pmemcpy/memcpy.h>
#include <mpi.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main() {
    init_pmemulator();
    int fd = open("test.txt", O_WRONLY | O_CREAT, 0666);
    write(fd, "hello", 4);
}