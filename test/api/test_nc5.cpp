#include <pnetcdf.h>
int main(int argc, char **argv) {
    int status, ncid, dimid;
    double *ddata=NULL;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Info info;
    char *path = argv[1];
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    status = ncmpi_create(comm,
      filename, NC_CLOBBER|NC_64BIT_OFFSET, info, &ncid);
    status = ncmpi_def_dim(ncid, "nx", (long)nx, &dimid);
    status = ncmpi_def_var (ncid, "A",
      NC_DOUBLE, 3, dimid, &cube1_id);
    status = ncmpi_enddef(ncid);
    status = ncmpi_put_vara_double_all(ncid, cube1_id,
                                       &offset, &count,
                                       (const void *)&(ddata[0]));
    status = ncmpi_close(ncid);
    MPI_Finalize ();
    return 0;
}
