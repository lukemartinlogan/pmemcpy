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

// 1. all vars (use restart)
int read_pattern_1 (int argc, char ** argv)
{
    printf ("not implemented\n");
}

// 2. all of 1 var
int read_pattern_2 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], nx_str [256], ny_str [256], nz_str [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid;

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    int dims [10];
    int dim_val;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (nx_str, argv [3]);
    strcpy (ny_str, argv [4]);
    strcpy (nz_str, argv [5]);
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
    pmem.load<long>("nx", &nx);
    pmem.load<long>("ny", &ny);
    pmem.load<long>("nz", &nz);

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
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %d %lf\n", filename, 2, nproc_x, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

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
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
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

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (nx_str, argv [3]);
    strcpy (ny_str, argv [4]);
    strcpy (nz_str, argv [5]);
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
    pmem.load<long>("nx", &nx);
    pmem.load<long>("ny", &ny);
    pmem.load<long>("nz", &nz);

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

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_y_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_z_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    pmem.load<double>("D", grav_x_c, start, readsize);
    pmem.load<double>("E", grav_y_c, start, readsize);
    pmem.load<double>("F", grav_z_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %d %lf\n", filename, 3, nproc_x, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 4. 1 plane in each dimension for 1 variable
int read_pattern_4 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], nx_str [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (nx_str, argv [3]);
    strcpy (ny_str, argv [4]);
    //strcpy (nz_str, argv [5]);
    nproc_x = atoi (nx_str);
    nproc_y = atoi (ny_str);
    //nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", &nx);
    pmem.load<long>("ny", &ny);
    pmem.load<long>("nz", &nz);

    my_x_dim = ny / nproc_x;
    my_y_dim = nz / nproc_y;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = nx / 2;
    start [1] = x_min;
    start [2] = y_min;
    readsize [0] = 1;
    readsize [1] = my_x_dim;
    readsize [2] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(2)
    my_x_dim = nx / nproc_x;
    my_y_dim = nz / nproc_y;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = x_min;
    start [1] = ny / 2;
    start [2] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = 1;
    readsize [2] = my_y_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    my_x_dim = nx / nproc_x;
    my_y_dim = ny / nproc_y;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = x_min;
    start [1] = y_min;
    start [2] = nz / 2;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    readsize [2] = 1;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 4, nproc_x, nproc_y, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 5. an arbitrary rectangular area (full dimensions)
int read_pattern_5 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], nx_str [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    double * grav_x_c = 0;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (nx_str, argv [3]);
    strcpy (ny_str, argv [4]);
    strcpy (nz_str, argv [5]);
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
    pmem.load<long>("nx", &nx);
    pmem.load<long>("ny", &ny);
    pmem.load<long>("nz", &nz);

    my_x_dim = nx / nproc_x / 2;
    my_y_dim = ny / nproc_y / 2;
    my_z_dim = nz / nproc_z / 2;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = ((rank/nproc_x) % nproc_y) * my_y_dim;
    z_min = rank/(nproc_x*nproc_y) * my_z_dim;
    start [0] = nx / 4 + x_min;
    start [1] = ny / 4 + y_min;
    start [2] = nz / 4 + z_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    readsize [2] = my_z_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %d %lf\n", filename, 5, nproc_x, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
int read_pattern_6 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], nx_str [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_x;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int my_z_dim;    // size of local z
    int x_min;       // offset for local x
    int y_min;       // offset for local y
    int z_min;       // offset for local z

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (nx_str, argv [3]);
    strcpy (ny_str, argv [4]);
    //strcpy (nz_str, argv [5]);
    nproc_x = atoi (nx_str);
    nproc_y = atoi (ny_str);
    //nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", &nx);
    pmem.load<long>("ny", &ny);
    pmem.load<long>("nz", &nz);

    my_x_dim = ny / nproc_x / 2;
    my_y_dim = nx / nproc_y / 2;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = nx / 4 + y_min;
    start [1] = ny / 4 + x_min;
    start [2] = nz / 4;
    readsize [0] = my_y_dim;
    readsize [1] = my_x_dim;
    readsize [2] = 1;
    pmem.load<double>("D", grav_x_c, start, readsize);

    // read in dim(2)
    my_x_dim = nz / nproc_x / 2;
    my_y_dim = nx / nproc_y / 2;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = nx / 4 + y_min;
    start [1] = ny / 4;
    start [2] = nz / 4 + x_min;
    readsize [0] = my_y_dim;
    readsize [1] = 1;
    readsize [2] = my_x_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    // read in dim(3)
    my_x_dim = nz / nproc_x / 2;
    my_y_dim = ny / nproc_y / 2;
    x_min = (rank % nproc_x) * my_x_dim;
    y_min = (rank/nproc_x) * my_y_dim;
    start [0] = nx / 4;
    start [1] = ny / 4 + y_min;
    start [2] = nz / 4 + x_min;
    readsize [0] = 1;
    readsize [1] = my_y_dim;
    readsize [2] = my_x_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("D", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 6, nproc_x, nproc_y, end_time - start_time);

    nc_err = MPI_Finalize ();
}

int main (int argc, char ** argv)
{
    int pattern;

    pattern = atoi (argv [1]);

// 1. all vars (use restart)
// 2. all of 1 var
// 3. all of a few vars (3 for 3-d, for example)
// 4. 1 plane in each dimension for 1 variable
// 5. an arbitrary rectangular area (full dimensions)
// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
    switch (pattern)
    {
        case 1:
            read_pattern_1 (argc, argv);
            break;

        case 2:
            read_pattern_2 (argc, argv);
            break;

        case 3:
            read_pattern_3 (argc, argv);
            break;

        case 4:
            read_pattern_4 (argc, argv);
            break;

        case 5:
            read_pattern_5 (argc, argv);
            break;

        case 6:
            read_pattern_6 (argc, argv);
            break;
    }

    return 0;
}
