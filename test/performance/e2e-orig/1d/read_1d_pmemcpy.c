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
#include <stdint.h>

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
    char filename [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int dims [10];
    int dim_val;

    MPI_Offset nparam, ntracke, ntracki;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.store<long>("nparam", &nparam);
    pmem.store<long>("ntracke", &ntracke);
    pmem.store<long>("ntracki", &ntracki);

    my_x_dim = nparam;
    my_y_dim = ntracke / size;
    x_min = 0;
    y_min = rank * my_y_dim;
    start [0] = x_min;
    start [1] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * my_x_dim * my_y_dim);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 2, size, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nparam_dimid, ntracke_dimid, ntracki_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;

    int rank;
    int size;
    MPI_Offset nparam;
    MPI_Offset ntracke;
    MPI_Offset ntracki;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    double * grav_x_c;
    double * grav_y_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.store<long>("nparam", &nparam);
    pmem.store<long>("ntracke", &ntracke);
    pmem.store<long>("ntracki", &ntracki);

    my_x_dim = nparam;
    my_y_dim = ntracke / size;
    x_min = 0;
    y_min = rank * my_y_dim;
    start [0] = x_min;
    start [1] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    grav_y_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);
    pmem.load<double>("ptrackedi", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 3, size, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 4. 1 plane in each dimension for 1 variable
int read_pattern_4 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256];
    char dim_name [100];

    int nparam_dimid, ntracke_dimid, ntracki_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    MPI_Offset nparam;
    MPI_Offset ntracke;
    MPI_Offset ntracki;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.store<long>("nparam", &nparam);
    pmem.store<long>("ntracke", &ntracke);
    pmem.store<long>("ntracki", &ntracki);

    my_x_dim = 1;
    my_y_dim = ntracke / size;
    x_min = nparam / 2;
    y_min = rank * my_y_dim;
    start [0] = x_min;
    start [1] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    my_x_dim = nparam;
    my_y_dim = 1;
    x_min = 0;
    y_min = ntracke / 2;
    start [0] = x_min;
    start [1] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 4, size, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 5. an arbitrary rectangular area (full dimensions)
int read_pattern_5 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256];
    char dim_name [100];

    int nparam_dimid, ntracke_dimid, ntracki_dimid;

    int grav_x_c_varid, grav_y_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    MPI_Offset nparam;
    MPI_Offset ntracke;
    MPI_Offset ntracki;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    double * grav_x_c = 0;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.store<long>("nparam", &nparam);
    pmem.store<long>("ntracke", &ntracke);
    pmem.store<long>("ntracki", &ntracki);

    my_x_dim = nparam / 2;
    my_y_dim = ntracke / size / 2;
    x_min = nparam / 4;
    y_min = rank * my_y_dim;
    start [0] = x_min;
    start [1] = ntracke / 4 + y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 5, size, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
int read_pattern_6 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256];
    char dim_name [100];

    int nparam_dimid, ntracke_dimid, ntracki_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    MPI_Offset nparam;
    MPI_Offset ntracke;
    MPI_Offset ntracki;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.store<long>("nparam", &nparam);
    pmem.store<long>("ntracke", &ntracke);
    pmem.store<long>("ntracki", &ntracki);

    my_x_dim = nparam / 2;
    my_y_dim = 1;
    x_min = nparam / 4;
    y_min = ntracke / 2;
    start [0] = x_min;
    start [1] = y_min;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    my_x_dim = 1;
    my_y_dim = ntracke / size / 2;
    x_min = nparam / 2;
    y_min = ntracke / 4;
    start [0] = x_min;
    start [1] = y_min + rank * my_y_dim;
    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    pmem.load<double>("ptrackede", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 6, size, end_time - start_time);

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
