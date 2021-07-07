#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmemcpy/memcpy.h>
#include <string.h>
#include <omp.h>
#include "logger.h"

/* Prototype for functions used only in this file */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int status;
    int nprocs;
    uint64_t cube_count[3];
    int npx, npy, npz, ndx, ndy, ndz;
    uint64_t nx, ny, nz;
    double t_time, sz, gps;
    char filename [256];
    double start_time, end_time;
    pmemcpy::StorageType storage_t;
    pmemcpy::SerializerType serializer_t;
    int use_mmap;

    if(argc != 11) {
        printf("USAGE: npx npy npz ndx ndy ndz storage_type serializer_type use_mmap\n");
        exit(1);
    }

    npx = atoi(argv[2]);
    npy = atoi(argv[3]);
    npz = atoi(argv[4]);
    ndx = atoi(argv[5]);
    ndy = atoi(argv[6]);
    ndz = atoi(argv[7]);
    PMEMCPY_ERROR_HANDLE_START()
    storage_t = pmemcpy::StorageTypeConverter::convert(argv[8]);
    serializer_t = pmemcpy::SerializerTypeConverter::convert(argv[9]);
    PMEMCPY_ERROR_HANDLE_END()
    use_mmap = atoi(argv[10]);
    nprocs = npx*npy*npz;

    if(storage_t == pmemcpy::StorageType::POSIX) {
        sprintf (filename, "%s", argv [1]);
    } else {
        sprintf (filename, "%s.pmemcpy_omp", argv [1]);
    }

    nx = npx * ndx;
    ny = npy * ndy;
    nz = npz * ndz;

    pmemcpy::PMEM pmem(storage_t, serializer_t, use_mmap);
    PMEMCPY_ERROR_HANDLE_START()
    pmem.mmap(filename, (size_t) (55 * pmemcpy::SizeType::GB));
    //pmem.mmap(filename, (size_t) (15 * pmemcpy::SizeType::GB));
    PMEMCPY_ERROR_HANDLE_END()

    omp_set_dynamic(0);
    #pragma omp parallel shared(npx, npy, npz, ndx, ndy, ndz, nx, ny, nz, cube_count, start_time, end_time, t_time, sz, gps, pmem) num_threads(nprocs)
    {
        int  offx, offy, offz, posx, posy, posz;
        uint64_t cube_start[3];
        double *ddata;
        int rank;

        rank = omp_get_thread_num();
        posx = rank % npx;
        posy = (rank / npx) % npy;
        posz = rank / (npx * npy);

        offx = posx * ndx;
        offy = posy * ndy;
        offz = posz * ndz;

        ddata = (double *) malloc(sizeof(double) * ndx * ndy * ndz);

        for (int i=0;i<ndx*ndy*ndz;i++) {
            ddata [i] = (rank*i + (rank+1)*(rank+1)*i + (i+1)*(i+1)*rank);
        }

        cube_start[0] = offx;
        cube_start[1] = offy;
        cube_start[2] = offz;

        if(rank == 0) {
            cube_count[0] = ndx;
            cube_count[1] = ndy;
            cube_count[2] = ndz;
        }

#pragma omp barrier
        if (rank == 0) {
            start_time = MPI_Wtime();
        }
        if (rank == 0) {
            PMEMCPY_ERROR_HANDLE_START()
            pmem.store<uint64_t>("nx", nx);
            pmem.store<uint64_t>("ny", ny);
            pmem.store<uint64_t>("nz", nz);
            PMEMCPY_ERROR_HANDLE_END()
        }

        PMEMCPY_ERROR_HANDLE_START()
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
        PMEMCPY_ERROR_HANDLE_END()

    #pragma omp barrier
        if (rank == 0) {
            end_time = MPI_Wtime();
            free(ddata);
        }
    }
    //io_type method nprocs ndx ndy ndz size_per_proc agg_size time storage serializer
    log_end1((const char*)"write", (const char*)"pmemcpy_omp", nprocs, start_time, end_time, cube_count, argv[8], argv[9]);
    MPI_Finalize();
    return 0;
}

