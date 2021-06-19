#include <pmemulator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include <adios.h>
#include <adios_read.h>
#include <adios_read_v1.h>

int read_pattern_3(int argc, char ** argv) {
    init_pmemulator();

    int adios_err;
    ADIOS_FILE *adios_handle;
    ADIOS_GROUP *group_handle;
    ADIOS_VARINFO *var_info;
    uint64_t read_bytes;

    int nc_err;
    int ncid;
    char filename[256];
    char dim_name[100];
    uint64_t start[10];
    uint64_t readsize[10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;

    int rank;
    int npx, npy, npz;
    int nx, ny, nz;
    int ndx, ndy, ndz;    // size of local z
    int posx, posy, posz;       // offset for local z
    int offx, offy, offz;       // offset for local z
    int nprocs;

    double *grav_x_c;
    double *grav_y_c;
    double *grav_z_c;

    double start_time, end_time;

    adios_err = MPI_Init(&argc, &argv);
    adios_err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf(filename, "%s.bp", argv[1]);
    npx = atoi(argv[2]);
    npy = atoi(argv[3]);
    npz = atoi(argv[4]);
    nprocs = npx*npy*npz;

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

    ndx = nx / npx ;
    ndy = ny / npy;
    ndz = nz/ npz;

    posx = rank%npx;
    posy = (rank/npx) %npy;
    posz = rank/(npx*npy);

    offx = posx * ndx;
    offy = posy * ndy;
    offz = posz * ndz;

    start[0] = offx;
    start[1] = offy;
    start[2] = offz;

    readsize[0] = ndx;
    readsize[1] = ndy;
    readsize[2] = ndz;

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
    //io_type method nprocs ndx ndy ndz size_per_proc agg_size time storage serializer
    if (rank == 0) {
        size_t size_per_proc = 3*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
        size_t agg_size = size_per_proc*nprocs;
        printf("read bp %lu %lu %lu %lu %lu %lu %lf none none\n", nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time);
    }
    adios_err = MPI_Finalize();
}

int main (int argc, char ** argv)
{
    read_pattern_3(argc, argv);
    return 0;
}
