#include <pmemcpy/pmemcpy.h>
int main(int argc, char** argv) {
    int rank, nprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    pmemcpy::PMEM pmem;
    size_t count = 100;
    size_t off = 100*rank;
    size_t dimsf = 100*nprocs;
    char *path = argv[1];

    double data[100] = {0};
    pmem.mmap(path, MPI_COMM_WORLD);
    pmem.alloc<double>("A", 1, &dimsf);
    pmem.store<double>("A", data, 1, &off, &count);
    MPI_Finalize();
}
