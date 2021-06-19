#include <pmemulator.h>
#include <mpi.h>
#include <pnetcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    init_pmemulator();

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
    MPI_Offset nx, ny, nz;
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

    sprintf (filename, "%s.nc5", argv [1]);
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

    nc_err = ncmpi_open (MPI_COMM_WORLD, filename, NC_NOWRITE|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 

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

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);
    grav_y_c = malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);
    grav_z_c = malloc (sizeof (double) * readsize [0] * readsize [1] * readsize [2]);

    nc_err = ncmpi_inq_varid (ncid, "D", &grav_x_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_varid (ncid, "E", &grav_y_c_varid);
    if (nc_err != NC_NOERR) fprintf(stderr, "%d: %s\n", rank, ncmpi_strerror (nc_err)); 
    nc_err = ncmpi_inq_varid (ncid, "F", &grav_z_c_varid);
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
    //io_type method nprocs ndx ndy ndz size_per_proc agg_size time storage serializer
    if (rank == 0) {
        size_t size_per_proc = 3*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
        size_t agg_size = size_per_proc*size;
        printf("read nc5 %d %lld %lld %lld %lu %lu %lf none none\n", size, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time);
    }
    nc_err = MPI_Finalize ();
}

int main (int argc, char ** argv)
{
    int pattern;
// 1. all vars (use restart)
// 2. all of 1 var
// 3. all of a few vars (3 for 3-d, for example)
// 4. 1 plane in each dimension for 1 variable
// 5. an arbitrary rectangular area (full dimensions)
// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
    read_pattern_3 (argc, argv);
    return 0;
}
