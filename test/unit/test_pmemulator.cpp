//
// Created by lukemartinlogan on 6/9/21.
//

#include <pmemcpy/memcpy.h>
#include <mpi.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv) {
    MPI_Init(&argc,&argv);
    int fd = open("test.txt", O_WRONLY | O_CREAT, 0666);
    write(fd, "hello", 4);
    MPI_Finalize();
}