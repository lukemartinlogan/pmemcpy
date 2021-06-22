#include <mpi.h>
#include <pmemcpy/memcpy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256];
    char dim_name [100];

    int rank;
    int size;
    int nproc_x, nproc_y, nproc_z;

    MPI_Offset readsize [10];

    double start_time, end_time;
    pmemcpy::StorageType storage_t;
    pmemcpy::SerializerType serializer_t;
    int use_mmap;

    if(argc != 8) {
        printf("USAGE: filename npx npy npz storage_type serializer_type use_mmap\n");
        exit(1);
    }

    nproc_x = atoi ( argv [2]);
    nproc_y = atoi (argv [3]);
    nproc_z = atoi (argv [4]);
    PMEMCPY_ERROR_HANDLE_START()
    storage_t = pmemcpy::StorageTypeConverter::convert(argv[5]);
    serializer_t = pmemcpy::SerializerTypeConverter::convert(argv[6]);
    PMEMCPY_ERROR_HANDLE_END()
    use_mmap = atoi(argv[7]);
    size = nproc_x * nproc_y * nproc_z;
    if(storage_t == pmemcpy::StorageType::POSIX) {
        sprintf (filename, "%s", argv [1]);
    } else {
        sprintf (filename, "%s.pmemcpy_omp", argv [1]);
    }

    pmemcpy::PMEM pmem(storage_t, serializer_t, use_mmap);
    PMEMCPY_ERROR_HANDLE_START()
    pmem.mmap(filename);
    PMEMCPY_ERROR_HANDLE_END()

    omp_set_dynamic(0);
#pragma omp parallel shared(nproc_x, nproc_y, nproc_z, readsize, nc_err, start_time, end_time, filename, pmem) num_threads(size)
    {
        int my_x_dim;    // size of local x
        int my_y_dim;    // size of local y
        int my_z_dim;    // size of local z
        int x_min;       // offset for local x
        int y_min;       // offset for local y
        int z_min;       // offset for local z
        uint64_t nx, ny, nz;

        MPI_Offset local_readsize[10];
        std::string tags[10] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
        double * grav_x_c;

        MPI_Offset start [10];

#pragma omp barrier
        int rank = omp_get_thread_num();
        if(rank == 0) {
            start_time = MPI_Wtime();
        }

        PMEMCPY_ERROR_HANDLE_START()
        pmem.load<uint64_t>("nx", nx);
        pmem.load<uint64_t>("ny", ny);
        pmem.load<uint64_t>("nz", nz);
        PMEMCPY_ERROR_HANDLE_END()

        my_x_dim = nx / nproc_x;
        my_y_dim = ny / nproc_y;
        my_z_dim = nz / nproc_z;
        x_min = (rank % nproc_x) * my_x_dim;
        y_min = ((rank / nproc_x) % nproc_y) * my_y_dim;
        z_min = rank / (nproc_x * nproc_y) * my_z_dim;
        start[0] = x_min;
        start[1] = y_min;
        start[2] = z_min;
        if(rank == 0) {
            readsize[0] = my_x_dim;
            readsize[1] = my_y_dim;
            readsize[2] = my_z_dim;
        }
        local_readsize[0] = my_x_dim;
        local_readsize[1] = my_y_dim;
        local_readsize[2] = my_z_dim;

        grav_x_c = (double *) malloc(sizeof(double) * local_readsize[0] * local_readsize[1] * local_readsize[2]);

        PMEMCPY_ERROR_HANDLE_START()
        for(int i = 0; i < 10; ++i) {
            pmem.load<double>(tags[i] + std::to_string(rank), grav_x_c,
                              pmemcpy::Dimensions(local_readsize[0], local_readsize[1], local_readsize[2]));
        }
        PMEMCPY_ERROR_HANDLE_END()

#pragma omp barrier
        if(rank == 0) {
            end_time = MPI_Wtime ();
        }
    }

    //io_type method size ndx ndy ndz size_per_proc agg_size time storage serializer
    size_t size_per_proc = 10*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
    size_t agg_size = size_per_proc*size;
    printf("read pmemcpy_omp %d %lld %lld %lld %lu %lu %lf %s %s\n",
           size, readsize[0], readsize[1], readsize[2],
           size_per_proc, agg_size, end_time - start_time,
           argv[5], argv[6]);
    return 0;
}

int main (int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    read_pattern_3 (argc, argv);
    MPI_Finalize();
    return 0;
}
