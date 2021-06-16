#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include <adios.h>

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
    uint64_t adios_groupsize, adios_totalsize;
    MPI_Comm comm = MPI_COMM_WORLD;

    sprintf (filename, "%s.bp", argv [1]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    adios_init ("3d.xml");

    if (argc < 7)
    {
        if (rank == 0)
        {
            fprintf (stderr, "usage: %s filename <proc counts> <local sizes>\n", argv [0]);
        }
        MPI_Finalize ();
        return 1;
    }

    npx = atoi(argv[2]);
    npy = atoi(argv[3]);
    npz = atoi(argv[4]);

    ndx = atoi(argv[5]);
    ndy = atoi(argv[6]);
    ndz = atoi(argv[7]);
  
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

    for (i=0;i<ndx*ndy*ndz;i++)
    {
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

//printf ("%03d start(0) %3d start(1) %3d start(2) %3d size(0) %3d size(1) %3d size(2) %3d\n", rank, cube_start[0], cube_start[1], cube_start[2], cube_count[0], cube_count[1], cube_count[2]);

    // start the timing now.
    status = MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    status = adios_open (&adios_handle, "data", filename, "w", &comm);
#include "gwrite_data.ch"
    status = adios_close (adios_handle);

    status = MPI_Barrier (MPI_COMM_WORLD);

    end_time = MPI_Wtime ();

    t_time = end_time - start_time;
    sz = (8.0 * 8.0 * nx * ny * ((double) nz)) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %d %d %d %lf %lf %lf\n", filename, nprocs, ndx, ndy, ndz, sz, t_time, gps);

    free (ddata);
    adios_finalize (rank);
    MPI_Finalize ();

    return 0;
}

