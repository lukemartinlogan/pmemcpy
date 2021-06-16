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
    char filename [256], ny_str [256], nz_str [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid;

    int rank;
    int size;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_j_ray_dim;
    int my_k_ray_dim;
    int j_ray_min;
    int k_ray_min;

    int dims [10];
    int dim_val;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", nx);
    pmem.load<long>("ny", ny);
    pmem.load<long>("nz", nz);

    my_j_ray_dim = ny / nproc_y;
    my_k_ray_dim = nz / nproc_z;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;

    start [0] = 0;
    start [1] = j_ray_min;
    start [2] = k_ray_min;
    readsize [0] = nx;
    readsize [1] = my_j_ray_dim;
    readsize [2] = my_k_ray_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 2, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], ny_str [256], nz_str [256];
    char dim_name [100];
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;

    int rank;
    int size;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_j_ray_dim;
    int my_k_ray_dim;
    int j_ray_min;
    int k_ray_min;

    double * grav_x_c;
    double * grav_y_c;
    double * grav_z_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", nx);
    pmem.load<long>("ny", ny);
    pmem.load<long>("nz", nz);

    my_j_ray_dim = ny / nproc_y;
    my_k_ray_dim = nz / nproc_z;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = 0;
    start [1] = j_ray_min;
    start [2] = k_ray_min;
    readsize [0] = nx;
    readsize [1] = my_j_ray_dim;
    readsize [2] = my_k_ray_dim;

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_y_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_z_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);
    pmem.load<double>("grav_y_c", grav_x_c, start, readsize);
    pmem.load<double>("grav_z_c", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 3, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 4. 1 plane in each dimension for 1 variable
int read_pattern_4 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_j_ray_dim;
    int my_k_ray_dim;
    int j_ray_min;
    int k_ray_min;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", nx);
    pmem.load<long>("ny", ny);
    pmem.load<long>("nz", nz);

    my_j_ray_dim = ny / nproc_y;
    my_k_ray_dim = nz / nproc_z;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = nx / 2;
    start [1] = j_ray_min;
    start [2] = k_ray_min;
    readsize [0] = 1;
    readsize [1] = my_j_ray_dim;
    readsize [2] = my_k_ray_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    my_j_ray_dim = nx / nproc_y;
    my_k_ray_dim = nz / nproc_z;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = j_ray_min;
    start [1] = ny / 2;
    start [2] = k_ray_min;
    readsize [0] = my_j_ray_dim;
    readsize [1] = 1;
    readsize [2] = my_k_ray_dim;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    my_j_ray_dim = ny / nproc_y;
    my_k_ray_dim = nx / nproc_z;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = k_ray_min;
    start [1] = j_ray_min;
    start [2] = nz / 2;
    readsize [0] = my_k_ray_dim;
    readsize [1] = my_j_ray_dim;
    readsize [2] = 1;
    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 4, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 5. an arbitrary rectangular area (full dimensions)
int read_pattern_5 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_j_ray_dim;
    int my_k_ray_dim;
    int j_ray_min;
    int k_ray_min;

    double * grav_x_c = 0;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", nx);
    pmem.load<long>("ny", ny);
    pmem.load<long>("nz", nz);

    my_j_ray_dim = ny / nproc_y / 2;
    my_k_ray_dim = nz / nproc_z / 2;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = nx / 4;
    start [1] = ny / 4 + j_ray_min;
    start [2] = nz / 4 + k_ray_min;
    readsize [0] = nx / 2;
    readsize [1] = my_j_ray_dim;
    readsize [2] = my_k_ray_dim;
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 5, nproc_y, nproc_z, end_time - start_time);

    nc_err = MPI_Finalize ();
}

// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
int read_pattern_6 (int argc, char ** argv)
{
    int nc_err;
    int ncid;
    char filename [256], ny_str [256], nz_str [256];
    char dim_name [100];

    int nx_dimid, ny_dimid, nz_dimid;

    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    MPI_Offset start [10];
    MPI_Offset readsize [10];

    int rank;
    int size;
    int nproc_y;
    int nproc_z;
    MPI_Offset nx;
    MPI_Offset ny;
    MPI_Offset nz;
    int my_j_ray_dim;
    int my_k_ray_dim;
    int j_ray_min;
    int k_ray_min;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.nc5", argv [2]);
    strcpy (ny_str, argv [3]);
    strcpy (nz_str, argv [4]);
    nproc_y = atoi (ny_str);
    nproc_z = atoi (nz_str);

    nc_err = MPI_Init (&argc, &argv);
    nc_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    nc_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    pmem.load<long>("nx", nx);
    pmem.load<long>("ny", ny);
    pmem.load<long>("nz", nz);

   // do plane in dim(1)
    my_j_ray_dim = ny / nproc_y / 2;
    my_k_ray_dim = nz / nproc_z / 2;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = nx / 4;
    start [1] = ny / 4 + j_ray_min;
    start [2] = nz / 4 + k_ray_min;
    readsize [0] = 1;
    readsize [1] = my_j_ray_dim;
    readsize [2] = my_k_ray_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   // do plane in dim(2)
    my_j_ray_dim = nx / nproc_y / 2;
    my_k_ray_dim = nz / nproc_z / 2;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = nx / 4 + j_ray_min;
    start [1] = ny / 4;
    start [2] = nz / 4 + k_ray_min;
    readsize [0] = my_j_ray_dim;
    readsize [1] = 1;
    readsize [2] = my_k_ray_dim;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   // do plane in dim(3)
    my_j_ray_dim = ny / nproc_y / 2;
    my_k_ray_dim = nx / nproc_z / 2;
    j_ray_min = (rank % nproc_y) * my_j_ray_dim;
    k_ray_min = (rank/nproc_y) * my_k_ray_dim;
    start [0] = nx / 4 + k_ray_min;
    start [1] = ny / 4 + j_ray_min;
    start [2] = nz / 4;
    readsize [0] = my_k_ray_dim;
    readsize [1] = my_j_ray_dim;
    readsize [2] = 1;
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    pmem.load<double>("grav_x_c", grav_x_c, start, readsize);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %d %lf\n", filename, 6, nproc_y, nproc_z, end_time - start_time);

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
