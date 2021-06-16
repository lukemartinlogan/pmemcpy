/***********************************************************
 *
 * This test program writes a netCDF file using the parallel
 * netCDF library using MPI-IO. 
 *
 **********************************************************/
/*
cc test_write_alloc3d.c  -o genarray_pnc -I/ccs/home/sklasky/parallel-netcdf/jaguar/include -L/ccs/home/sklasky/parallel-netcdf/jaguar/lib -lpnetcdf
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmemcpy/memcpy.h>
#include <string.h>
#include <omp.h>

/* Prototype for functions used only in this file */

int main(int argc, char **argv) {
    int status;
    int nprocs;
    uint64_t npx, npy, npz, ndx, ndy, ndz;
    uint64_t nx, ny, nz;
    double t_time, sz, gps;
    char filename [256];
    double start_time, end_time;
    omp_set_dynamic(0);

    sprintf (filename, "%s.pmemcpy_omp", argv [1]);
    npx = atoi(argv[2]);
    npy = atoi(argv[3]);
    npz = atoi(argv[4]);
    ndx = atoi(argv[5]);
    ndy = atoi(argv[6]);
    ndz = atoi(argv[7]);
    nprocs = atoi(argv[8]);

    nx = npx * ndx;
    ny = npy * ndy;
    nz = npz * ndz;

    remove(filename);
    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::CAPNPROTO);
    pmem.mmap(filename, 8 * (1ul << 30));

    #pragma omp parallel shared(npx, npy, npz, ndx, ndy, ndz, nx, ny, nz, start_time, end_time, t_time, sz, gps, pmem) num_threads(nprocs)
    {
        int  offx, offy, offz, posx, posy, posz;
        uint64_t cube_start[3];
        uint64_t cube_count[3];
        double *ddata;
        int rank;

        rank = omp_get_thread_num();
        posx = rank % npx;
        posy = (rank / npx) % npy;
        posz = rank / (npx * npy);

        if(rank == 0) {
            printf("NUMTHREADS: %d\n", omp_get_num_threads());
        }

        offx = posx * ndx;
        offy = posy * ndy;
        offz = posz * ndz;

        ddata = (double *) malloc(sizeof(double *) * ndx * ndy * ndz);

        for (int i = 0; i < ndx * ndy * ndz; i++) {
            ddata[i] = rank;
        }

        cube_start[0] = offx;
        cube_start[1] = offy;
        cube_start[2] = offz;

        cube_count[0] = ndx;
        cube_count[1] = ndy;
        cube_count[2] = ndz;


        if (rank == 0) {
            start_time = MPI_Wtime();
        }
#pragma omp barrier

        if (rank == 0) {
            pmem.store<uint64_t>("nx", nx);
            pmem.store<uint64_t>("ny", ny);
            pmem.store<uint64_t>("nz", nz);
        }

        pmem.store("A" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("B" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("C" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("D" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("E" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("F" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("G" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("H" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("I" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
        pmem.store("J" + std::to_string(rank), &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));

    #pragma omp barrier
        if (rank == 0) {
            end_time = MPI_Wtime();
            free(ddata);
        }
    }

    t_time = end_time - start_time;
    sz = (8.0 * 8.0 * nx * ny * ((double) nz)) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    printf("fn=%s nprocs=%lu ndx=%lu ndy=%lu ndz=%lu sz=%lf time=%lf gps=%lf\n", filename, nprocs, ndx, ndy, ndz, sz, t_time, gps);
    return 0;
}

