#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include <adios.h>
#include <adios_read.h>
#include <adios_read_v1.h>

int read_pattern_3(int argc, char ** argv) {
    int adios_err;
    ADIOS_FILE *adios_handle;
    ADIOS_GROUP *group_handle;
    ADIOS_VARINFO *var_info;
    uint64_t read_bytes;

    int nc_err;
    int ncid;
    char filename[256], nx_str[256], ny_str[256], nz_str[256];
    char dim_name[100];
    uint64_t start[10];
    uint64_t readsize[10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    double *grav_x_c;
    double *grav_y_c;
    double *grav_z_c;

    double start_time, end_time;

    adios_err = MPI_Init(&argc, &argv);
    adios_err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size(MPI_COMM_WORLD, &size);

    sprintf(filename, "%s.bp", argv[1]);
    strcpy(nx_str, argv[2]);
    strcpy(ny_str, argv[3]);
    strcpy(nz_str, argv[4]);
    nproc_x = atoi(nx_str);
    nproc_y = atoi(ny_str);
    nproc_z = atoi(nz_str);

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    adios_handle = adios_fopen(filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg());

    group_handle = adios_gopen(adios_handle, "data");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg());

    adios_err = MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    read_bytes = adios_read_var(group_handle, "nx", start, readsize, &nx);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg());
    read_bytes = adios_read_var(group_handle, "ny", start, readsize, &ny);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg());
    read_bytes = adios_read_var(group_handle, "nz", start, readsize, &nz);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg());

    my_x_dim = nx / nproc_x;
    my_y_dim = ny / nproc_y;
    my_z_dim = nz / nproc_z;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = ((rank / nproc_x) % nproc_y) * my_y_dim;
    z_min = rank / (nproc_x * nproc_y) * my_z_dim;

    start[0] = x_min;
    start[1] = y_min;
    start[2] = z_min;

    readsize[0] = my_x_dim;
    readsize[1] = my_y_dim;
    readsize[2] = my_z_dim;

    grav_x_c = malloc(sizeof(double) * readsize[0] * readsize[1] * readsize[2]);
    grav_y_c = malloc(sizeof(double) * readsize[0] * readsize[1] * readsize[2]);
    grav_z_c = malloc(sizeof(double) * readsize[0] * readsize[1] * readsize[2]);

    var_info = adios_inq_var(group_handle, "D");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg());

    read_bytes = adios_read_var(group_handle, "D", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg());
    read_bytes = adios_read_var(group_handle, "E", start, readsize, grav_y_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg());
    read_bytes = adios_read_var(group_handle, "F", start, readsize, grav_z_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg());

    adios_err = adios_gclose(group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg());
    adios_err = adios_fclose(adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg());

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (rank == 0)
        printf ("fn=%s dim=%d npx=%d npy=%d npz=%d time=%lf\n", filename, 3, nproc_x, nproc_y, nproc_z, end_time - start_time);
    adios_err = MPI_Finalize();
}

int main (int argc, char ** argv)
{
    read_pattern_3(argc, argv);
    return 0;
}
