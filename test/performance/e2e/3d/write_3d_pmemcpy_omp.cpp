#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmemcpy/memcpy.h>
#include <string.h>
#include <omp.h>

/* Prototype for functions used only in this file */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int status;
    int nprocs;
    uint64_t cube_count[3];
    uint64_t npx, npy, npz, ndx, ndy, ndz;
    uint64_t nx, ny, nz;
    double t_time, sz, gps;
    char filename [256];
    double start_time, end_time;
    pmemcpy::StorageType storage_t;
    pmemcpy::SerializerType serializer_t;

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
    nprocs = npx*npy*npz;

    if(storage_t == pmemcpy::StorageType::POSIX) {
        sprintf (filename, "%s", argv [1]);
    } else {
        sprintf (filename, "%s.pmemcpy_omp", argv [1]);
    }

    nx = npx * ndx;
    ny = npy * ndy;
    nz = npz * ndz;

    pmemcpy::PMEM pmem(storage_t, serializer_t);
    PMEMCPY_ERROR_HANDLE_START()
    pmem.mmap(filename, (size_t) (70 * pmemcpy::SizeType::GB));
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

        for (int i = 0; i < ndx * ndy * ndz; i++) {
            ddata[i] = rank;
        }

        cube_start[0] = offx;
        cube_start[1] = offy;
        cube_start[2] = offz;

        if(rank == 0) {
            cube_count[0] = ndx;
            cube_count[1] = ndy;
            cube_count[2] = ndz;
        }


        if (rank == 0) {
            start_time = MPI_Wtime();
        }
#pragma omp barrier

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
    size_t size_per_proc = 10*sizeof(double)*cube_count[0]*cube_count[1]*cube_count[2];
    size_t agg_size = size_per_proc*nprocs;
    printf("write pmemcpy_omp %d %lu %lu %lu %lu %lu %lf %s %s\n",
           nprocs, cube_count[0], cube_count[1], cube_count[2],
           size_per_proc, agg_size,
           end_time - start_time,
           argv[8], argv[9]);
    MPI_Finalize();
    return 0;
}

