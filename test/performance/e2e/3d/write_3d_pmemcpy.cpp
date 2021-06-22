#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmemcpy/memcpy.h>
#include <string.h>

/* Prototype for functions used only in this file */

int main(int argc, char **argv) {
    int status, nodecomm, nodesize, noderank;
    int rank;
    int nprocs;
    uint64_t npx, npy, npz, ndx, ndy, ndz;
    double t_time, sz, gps;
    MPI_Info info;
    char filename [256];

    uint64_t nx, ny, nz;
    int  offx, offy, offz, posx, posy, posz;
    MPI_Offset cube_start[3];
    MPI_Offset cube_count[3];
    double start_time, end_time;
    double *ddata;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Info_create(&info);
    MPI_Info_set(info, "romio_ds_write", "disable");

    sprintf (filename, "%s.pmemcpy", argv [1]);
    npx = atoi(argv[2]);
    npy = atoi(argv[3]);
    npz = atoi(argv[4]);
    ndx = atoi(argv[5]);
    ndy = atoi(argv[6]);
    ndz = atoi(argv[7]);

    MPI_Barrier(MPI_COMM_WORLD);

    nx = npx * ndx;
    ny = npy * ndy;
    nz = npz * ndz;

    posx = rank % npx;
    posy = (rank / npx) % npy;
    posz = rank / (npx * npy);

    offx = posx * ndx;
    offy = posy * ndy;
    offz = posz * ndz;

    ddata = (double *) malloc(sizeof(double *) * ndx * ndy * ndz);

    for (int i = 0; i < ndx * ndy * ndz; i++) {
        ddata[i] = rank*i + (rank+1)*(rank+1)*i + (i+1)*(i+1)*rank;
    }

    cube_start[0] = offx;
    cube_start[1] = offy;
    cube_start[2] = offz;

    cube_count[0] = ndx;
    cube_count[1] = ndy;
    cube_count[2] = ndz;

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::CAPNPROTO);
    pmem.mmap(filename, 70* (1ul << 30));

    if (rank == 0) {
        status = MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();
    }

    if (rank == 0) {
        pmem.store<uint64_t>("nx", nx);
        pmem.store<uint64_t>("ny", ny);
        pmem.store<uint64_t>("nz", nz);
    }

    pmem.store("A", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("B", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("C", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("D", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("E", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("F", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("G", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("H", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("I", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));
    pmem.store("J", &ddata[0], pmemcpy::Dimensions(ndx, ndy, ndz));

    status = MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        end_time = MPI_Wtime();
        t_time = end_time - start_time;
    }

    sz = (8.0 * 8.0 * nx * ny * ((double) nz)) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf("fn=%s nprocs=%d ndx=%lu ndy=%lu ndz=%lu sz=%lf time=%lf gps=%lf\n", filename, nprocs, ndx, ndy, ndz,
    sz, t_time, gps);
    free(ddata);

    MPI_Info_free(&info);
    MPI_Finalize();
    return 0;
}

