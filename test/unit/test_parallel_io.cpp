//
// Created by lukemartinlogan on 6/9/21.
//

#include <pmemcpy/memcpy.h>
#include <mpi.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

void test_posix(char *path) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for(int i = 0; i < 8; ++i) {
        if(rank == i) {
            int fd = open(path, O_RDWR | O_CREAT, 0x666);
            if (fd < 0) {
                perror("Open failed");
            } else {
                printf("OPEN SUCCEEDED\n");
            }
            void * data = mmap(NULL, 100*(1<<20), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if(data == MAP_FAILED) {
                perror("MMAP failed");
            } else {
                printf("MMAP SUCCEEDED\n");
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void test_mmap(char *path) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::CAPNPROTO);
    for(int i = 0; i < 8; ++i) {
        if (rank == i) {
            try {
                printf("MMAP FOR RANK %d\n", rank);
                pmem.mmap(path, 100 * (1 << 20));
                printf("MMAP SUCCESS!\n");
            }
            catch(...) {
                printf("MMAP FAILED!\n");
            }
        }
        printf("BARRIER %d\n", rank);
        MPI_Barrier(MPI_COMM_WORLD);
        sleep(1);
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    if(argc != 3) {
        printf("USAGE: test_pmem [pool-path] [test-case]");
        return -1;
    }
    char *path = argv[1];
    int test = atoi(argv[2]);

    //sleep(10);
    switch(test) {
        case 0: {
            test_posix(path);
            break;
        }
        case 1: {
            test_mmap(path);
            break;
        }
    }

    MPI_Finalize();
}