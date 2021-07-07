#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include <adios.h>
#include "logger.h"

int main(int argc, char **argv)
{
    int i, j, k, l;
    int cube_dim[3] ;
    int npx, npy, npz, ndx, ndy, ndz, nx, ny, nz;
    int offx, offy, offz, posx, posy, posz;
    int offsets[3], lsize[3], gsize[3];
    uint64_t cube_start[3];
    uint64_t cube_count[3];
    double *ddata=NULL;
    double * A, * B, * C, * D, * E, * F, * G, * H, * I, * J;
    int rank;
    int nprocs;
    double start_time, end_time, t_time, sz, gps;
    int time;
    int status;
    char filename [256];

    int64_t adios_handle;
    uint64_t adios_groupsize=0, adios_totalsize = 0;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf (filename, "%s.bp", argv [1]);
    adios_init (argv[2], MPI_COMM_WORLD); //3d.xml

    npx = atoi(argv[3]);
    npy = atoi(argv[4]);
    npz = atoi(argv[5]);
    nprocs = npx*npy*npz;

    ndx = atoi(argv[6]);
    ndy = atoi(argv[7]);
    ndz = atoi(argv[8]);
  
    nx = npx * ndx;
    ny = npy * ndy;
    nz = npz * ndz;
  
    posx = rank%npx;
    posy = (rank/npx) %npy;
    posz = rank/(npx*npy);
  
    offx = posx * ndx;
    offy = posy * ndy;
    offz = posz * ndz;

    ddata = (double *)malloc(sizeof(double)*ndx*ndy*ndz);

    srand(1000);
    for (i=0;i<ndx*ndy*ndz;i++) {
       //ddata [i] = (rank*i + (rank+1)*(rank+1)*i + (i+1)*(i+1)*rank)*rand();
       ddata [i] = rank;
    }

    A = B = C = D = E = F = G = H = I = J = ddata;

    cube_start[0] = offx;
    cube_start[1] = offy;
    cube_start[2] = offz;
 
    cube_count[0] = ndx;
    cube_count[1] = ndy;
    cube_count[2] = ndz;
    // the number of procs must be divisible by 8


    // start the timing now.
    status = MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    status = adios_open (&adios_handle, "data", filename, "w", comm);
    adios_groupsize = 36lu + 80lu*ndx*ndy*ndz;
    adios_group_size (adios_handle, adios_groupsize, &adios_totalsize);
    adios_write (adios_handle, "nx", &nx);
    adios_write (adios_handle, "ny", &ny);
    adios_write (adios_handle, "nz", &nz);
    adios_write (adios_handle, "offx", &offx);
    adios_write (adios_handle, "offy", &offy);
    adios_write (adios_handle, "offz", &offz);
    adios_write (adios_handle, "ndx", &ndx);
    adios_write (adios_handle, "ndy", &ndy);
    adios_write (adios_handle, "ndz", &ndz);
    adios_write (adios_handle, "A", A);
    adios_write (adios_handle, "B", B);
    adios_write (adios_handle, "C", C);
    adios_write (adios_handle, "D", D);
    adios_write (adios_handle, "E", E);
    adios_write (adios_handle, "F", F);
    adios_write (adios_handle, "G", G);
    adios_write (adios_handle, "H", H);
    adios_write (adios_handle, "I", I);
    adios_write (adios_handle, "J", J);
    status = adios_close (adios_handle);

    status = MPI_Barrier (MPI_COMM_WORLD);

    end_time = MPI_Wtime ();
    //io_type method nprocs ndx ndy ndz size_per_proc agg_size time storage serializer
    if (rank == 0) {
        log_end1("write", "bp", nprocs, start_time, end_time, cube_count, "none", "none");
    }
    free (ddata);
    adios_finalize (rank);
    MPI_Finalize ();

    return 0;
}

