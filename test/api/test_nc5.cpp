#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnetcdf.h>
#include <string.h>
#define _GNU_SOURCE
#include <unistd.h>
#include "logger.h"

/* Prototype for functions used only in this file */
static void handle_error(int status);

static void handle_error(int status) {
    fprintf(stderr, "%s\n", ncmpi_strerror(status));
}

int main(int argc, char **argv) {
    int status;
    int ncid;
    int dimid;
    int cube_dim[3];
    static char title[] = "example netCDF dataset";
    static char description[] = "3-D integer array";
    double *ddata=NULL;

    MPI_Comm comm = MPI_COMM_WORLD;
    char oname[128];
    double start_time, end_time, t_time,sz, gps;
    MPI_Info info;
    MPI_File fh;
    int time;
    char filename [256];

    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    status = ncmpi_create(comm, filename, NC_CLOBBER|NC_64BIT_OFFSET, info, &ncid);
    status = ncmpi_put_att_text (ncid, NC_GLOBAL, "title",
                                 strlen(title), title);
    status = ncmpi_def_dim(ncid, "nx", (long)nx, &dimid);
    status = ncmpi_def_var (ncid, "A", NC_DOUBLE, 3, dimid, &cube1_id);
    status = ncmpi_enddef(ncid);
    status = ncmpi_put_vara_double_all(ncid, cube1_id,
                                       &offset, &count,
                                       (const void *)&(ddata[0]));
    status = ncmpi_close(ncid);
    MPI_Finalize ();
    return 0;
}

