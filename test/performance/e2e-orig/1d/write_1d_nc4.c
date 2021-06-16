//***********************************************************
//*
//* This test program writes a netCDF file using the parallel
//* netCDF library using MPI-IO. 
//*
//***********************************************************
//cc test_writeoc3d4.c  -o genarray_pnc4 ${NETCDF_INCLUDE_OPTS} -L${NETCDF_DIR}/lib/netcdf 



#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>
#include <stdint.h>

static void handle_error (int status)
{
    fprintf (stderr, "%s\n", nc_strerror (status));
}

int main(int argc, char **argv)
{
    int status;
    int ncid;
    int dimid1, dimid2, dimid3;
    int particles_dim [2];
    size_t particles_start [2];
    size_t particles_count [2];
    int particles1_id, particles2_id, mype_id;
    static char title [] = "example netCDF dataset";
    static char description [] = "3-D integer array";
    int nparam, mype;
    long ntracke, ntracki, offset_i, offset_e;
    int l_ntracke, l_ntracki;
    double * ddata_i = NULL;
    double * ddata_e = NULL;
    int rank;
    int size;
    MPI_Comm comm = MPI_COMM_NULL;
    char filename [256];
    double start_time, end_time, t_time,sz, gps;
    MPI_Info info;
    MPI_File fh;
    int time;

    nparam = 8;
    int average_particle_count = 1000000;
    float average_variation = 0.2;

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Info_create (&info);
    MPI_Info_set (info, "cb_align", "2");
    MPI_Info_set (info, "romio_ds_write", "disable");
    MPI_Comm_dup (MPI_COMM_WORLD, &comm);
if (rank == 0) printf ("A\n");

    mype = rank;

    ntracki = ((long) average_particle_count) * size;
    ntracke = ((long) average_particle_count) * size;

    sprintf (filename, "%s.nc4", argv [1]);

    if ((rank % 2) == 0)
    {
        l_ntracke = average_particle_count * (1 - average_variation);
        offset_e = ((long) average_particle_count) * (rank);

        l_ntracki = average_particle_count * (1 - average_variation);
        offset_i = ((long) average_particle_count) * (rank);
    }
    else
    {
        l_ntracke = average_particle_count * (1 + average_variation);
        offset_e = ((rank - 1) * ((long) average_particle_count)) + ((rank % 2) * ((long) average_particle_count) * (1 - average_variation));

        l_ntracki = average_particle_count * (1 + average_variation);
        offset_i = ((rank - 1) * ((long) average_particle_count)) + ((rank % 2) * ((long) average_particle_count) * (1 - average_variation));
    }

    ddata_e = (double *) malloc (sizeof (double) * nparam * l_ntracke);
    ddata_i = (double *) malloc (sizeof (double) * nparam * l_ntracki);

    for (int i = 0; i < nparam * l_ntracke; i++)
        ddata_e [i] = rank;

    for (int i = 0; i < nparam * l_ntracki; i++)
        ddata_i [i] = rank;
  
    // start the timing now.
if (rank == 0) printf ("B\n");
  
    status = MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    status = nc_create_par (filename, NC_NETCDF4 | NC_MPIIO, comm, info, &ncid);
if (rank == 0) printf ("C\n");
 
    status = nc_def_dim (ncid, "nparam", (long) nparam, &dimid1);
    status = nc_def_dim (ncid, "ntracke", (long) ntracke, &dimid2);
    status = nc_def_dim (ncid, "ntracki", (long) ntracki, &dimid3);

    status = nc_def_var (ncid, "mype", NC_INT, 0, 0, &mype_id);
if (rank == 0) printf ("D\n");

    particles_dim[0] = dimid1;
    particles_dim[1] = dimid2;
    status = nc_def_var (ncid, "ptrackede", NC_DOUBLE, 2, particles_dim, &particles1_id);

    particles_dim[0] = dimid1;
    particles_dim[1] = dimid3;
    status = nc_def_var (ncid, "ptrackedi", NC_DOUBLE, 2, particles_dim, &particles2_id);

    status = nc_enddef (ncid);
if (rank == 0) printf ("E\n");
     
    status = nc_var_par_access (ncid, mype_id, NC_INDEPENDENT);
    status = nc_var_par_access (ncid, particles1_id, NC_COLLECTIVE);
    status = nc_var_par_access (ncid, particles2_id, NC_COLLECTIVE); 
if (rank == 0) printf ("F\n");

    if (rank == 0)
        status = nc_put_var_int (ncid, mype_id, &mype);
if (rank == 0) printf ("F1\n");

    particles_start [0] = 0;
    particles_start [1] = offset_e;
    particles_count [0] = nparam;
    particles_count [1] = l_ntracke;
//printf ("%03d locale %4d globale %4d offsete %4d locali %4d globali %4d offseti %4d\n", rank, l_ntracke / 100000, ntracke / 100000, offset_e / 100000, l_ntracki / 100000, ntracki / 100000, offset_i / 100000);

//if (rank == 0) printf ("F2\n");
printf ("F2: start [0]:% lld, start [1]: %lld count [0]: %lld count [1]: %lld\n", particles_start [0], particles_start [1], particles_count [0], particles_count [1]);
    status = nc_put_vara_double (ncid, particles1_id, particles_start, particles_count, ddata_e);
if (rank == 0) printf ("G\n");

    particles_start [0] = 0;
    particles_start [1] = offset_i;
    particles_count [0] = nparam;
    particles_count [1] = l_ntracki;
    status = nc_put_vara_double (ncid, particles2_id, particles_start, particles_count, ddata_e);
  
if (rank == 0) printf ("H\n");
    status = nc_close (ncid);
if (rank == 0) printf ("I\n");
  
    status = MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();
if (rank == 0) printf ("J\n");

    t_time = end_time - start_time;
    sz =  ((double) ntracki + ntracke) * nparam * sizeof (double) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %lf %lf %lf \n", filename, size, sz, t_time, gps);

    free (ddata_e);
    free (ddata_i);
    MPI_Comm_free (&comm);
if (rank == 0) printf ("K\n");
    MPI_Info_free (&info);
    MPI_Barrier (MPI_COMM_WORLD);
if (rank == 0) printf ("L\n");
    MPI_Finalize ();
if (rank == 0) printf ("M\n");

    return 0;
}
