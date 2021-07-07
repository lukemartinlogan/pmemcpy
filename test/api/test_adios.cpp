#include <adios.h>
int main(int argc, char **argv) {
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char *path = argv[1];
    char *config = argv[2];
    double data[100];
    int64_t adios_handle;
    size_t count = 100;
    size_t offset = 100*rank;
    size_t dimsf = 100*nprocs;

    adios_init(config, MPI_COMM_WORLD);
    adios_open (&adios_handle, "dataset",
      path, "w", MPI_COMM_WORLD);
    adios_write (adios_handle, "count", &count);
    adios_write (adios_handle, "dimsf", &dimsf);
    adios_write (adios_handle, "offset", &offset);
    adios_write (adios_handle, "A", data);
    adios_close (adios_handle);
    adios_finalize (rank);
    MPI_Finalize ();
    return 0;
}
