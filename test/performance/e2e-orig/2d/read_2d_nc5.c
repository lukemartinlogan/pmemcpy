// read patterns:
// 1. all vars (use restart)
// 2. all of 1 var
// 3. all of a few vars (3 for 3-d, for example)
// 4. 1 plane in each dimension for 1 variable
// 5. an arbitrary rectangular area (full dimensions)
// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
#include <mpi.h>
#include <pnetcdf.h>
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

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    nc_err = ncmpi_inq_dimid (ncid, "nx", &nx_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nx_dimid, dim_name, &nx);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "ny", &ny_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, ny_dimid, dim_name, &ny);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "nz", &nz_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nz_dimid, dim_name, &nz);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
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

//printf ("%2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_inq_vardimid (ncid, grav_x_c_varid, dims);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_close (ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    nc_err = ncmpi_inq_dimid (ncid, "nx", &nx_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nx_dimid, dim_name, &nx);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "ny", &ny_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, ny_dimid, dim_name, &ny);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "nz", &nz_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nz_dimid, dim_name, &nz);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
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

//printf ("%2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_y_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);
    grav_z_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_varid (ncid, "grav_y_c", &grav_y_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_varid (ncid, "grav_z_c", &grav_z_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_get_vara_double_all (ncid, grav_y_c_varid, start, readsize, grav_y_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_get_vara_double_all (ncid, grav_z_c_varid, start, readsize, grav_z_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_close (ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    nc_err = ncmpi_inq_dimid (ncid, "nx", &nx_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nx_dimid, dim_name, &nx);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "ny", &ny_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, ny_dimid, dim_name, &ny);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "nz", &nz_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nz_dimid, dim_name, &nz);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(1)
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

//printf ("1 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(2)
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

//printf ("2 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(3)
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

//printf ("3 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = ncmpi_close (ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    nc_err = ncmpi_inq_dimid (ncid, "nx", &nx_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nx_dimid, dim_name, &nx);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "ny", &ny_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, ny_dimid, dim_name, &ny);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "nz", &nz_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nz_dimid, dim_name, &nz);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

//printf ("%2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
//    if (rank == 0) then
//    write (*,*) 'dims(1) = ', dims (1)
//    write (*,*) 'dims(2) = ', dims (2)
//    write (*,*) 'dims(3) = ', dims (3)
//    endif

    nc_err = ncmpi_close (ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    nc_err = ncmpi_inq_dimid (ncid, "nx", &nx_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nx_dimid, dim_name, &nx);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "ny", &ny_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, ny_dimid, dim_name, &ny);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dimid (ncid, "nz", &nz_dimid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_dim (ncid, nz_dimid, dim_name, &nz);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

//printf ("1 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
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

//printf ("2 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
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

//printf ("3 %2d start[0] %3d start[1] %3d start[2] %3d size[0] %3d size[1] %3d size[2] %3d\n", rank, start [0], start [1], start [2], readsize [0], readsize [1], readsize [2]);

    grav_x_c = malloc (sizeof (double) * readsize [1] * readsize [2] * readsize [3]);

    nc_err = ncmpi_inq_varid (ncid, "grav_x_c", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

    nc_err = ncmpi_get_vara_double_all (ncid, grav_x_c_varid, start, readsize, grav_x_c);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = ncmpi_close (ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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
