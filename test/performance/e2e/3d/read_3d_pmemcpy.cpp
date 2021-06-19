// read patterns:
// 1. all vars (use restart)
// 2. all of 1 var
// 3. all of a few vars (3 for 3-d, for example)
// 4. 1 plane in each dimension for 1 variable
// 5. an arbitrary rectangular area (full dimensions)
// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
#include <mpi.h>
#include <pmemcpy/memcpy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], nx_str [256], ny_str [256], nz_str [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    uint64_t nx, ny, nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    double * grav_x_c;
    double * grav_y_c;
    double * grav_z_c;

    double start_time, end_time;

    sprintf (filename, "%s.pmemcpy", argv [1]);
    strcpy (nx_str, argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_x = atoi (nx_str);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<uint64_t>("nx", nx);
    pmem.load<uint64_t>("ny", ny);
    pmem.load<uint64_t>("nz", nz);

    my_x_dim = nx / nproc_x;
    my_y_dim = ny / nproc_y;
    my_z_dim = nz / nproc_z;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = ((rank/nproc_x) % nproc_y) * my_y_dim;
    z_min = rank/(nproc_x*nproc_y) * my_z_dim;
    start [0] = x_min;
    start [1] = y_min;
    start [2] = z_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    readsize [2] = my_z_dim;

    grav_x_c = (double*)malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);
    grav_y_c = (double*)malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);
    grav_z_c = (double*)malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);

    pmem.load<double>("D" + std::to_string(rank), grav_x_c, pmemcpy::Dimensions(my_x_dim, my_y_dim, my_z_dim));
    pmem.load<double>("E" + std::to_string(rank), grav_y_c, pmemcpy::Dimensions(my_x_dim, my_y_dim, my_z_dim));
    pmem.load<double>("F" + std::to_string(rank), grav_z_c, pmemcpy::Dimensions(my_x_dim, my_y_dim, my_z_dim));

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();
    if (rank == 0)
        printf ("fn=%s dim=%d npx=%d npy=%d npz=%d time=%lf\n", filename, 3, nproc_x, nproc_y, nproc_z, end_time - start_time);
    nc_err = MPI_Finalize ();
    return 0;
}

int main (int argc, char ** argv)
{
    int pattern;
    read_pattern_3 (argc, argv);
    return 0;
}
