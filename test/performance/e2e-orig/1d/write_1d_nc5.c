//***********************************************************
//*
//* This test program writes a netCDF file using the parallel
//* netCDF library using MPI-IO. 
//*
//***********************************************************
//
//cc test_write_alloc3d.c  -o genarray_pnc -I/ccs/home/sklasky/parallel-netcdf/jaguar/include -L/ccs/home/sklasky/parallel-netcdf/jaguar/lib -lpnetcdf

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnetcdf.h>
#include <string.h>

static void handle_error (int status)
{
    fprintf (stderr, "%s\n", ncmpi_strerror (status));
}

int main (int argc, char ** argv)
{
    int status;
    int ncid;
    int dimid1, dimid2, dimid3;
    int particles_dim [2];
    MPI_Offset particles_start [2];
    MPI_Offset particles_count [2];
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
    MPI_Comm comm = MPI_COMM_WORLD;
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
    //MPI_Info_set (info, "romio_cb_write", "disable");
    MPI_Info_set (info, "romio_ds_write", "disable");

    mype = rank;

    ntracki = average_particle_count * size;
    ntracke = average_particle_count * size;

    sprintf (filename, "%s.nc5", argv [1]);

    if ((rank % 2) == 0)
    {
        l_ntracke = average_particle_count * (1 - average_variation);
        offset_e = (average_particle_count) * (rank);

        l_ntracki = average_particle_count * (1 - average_variation);
        offset_i = (average_particle_count) * (rank);
    }
    else
    {
        l_ntracke = average_particle_count * (1 + average_variation);
        offset_e = ((rank - 1) * average_particle_count) + ((rank % 2) * average_particle_count * (1 - average_variation));

        l_ntracki = average_particle_count * (1 + average_variation);
        offset_i = ((rank - 1) * average_particle_count) + ((rank % 2) * average_particle_count * (1 - average_variation));
    }

    ddata_i = (double *) malloc (sizeof (double) * nparam * l_ntracki);
    ddata_e = (double *) malloc (sizeof (double) * nparam * l_ntracke);

    for (int i = 0; i < nparam * l_ntracke; i++)
        ddata_e [i] = rank;

    for (int i = 0; i < nparam * l_ntracki; i++)
        ddata_i [i] = rank;

    // start the timing now.
  
    status = MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    status = ncmpi_create (comm, filename, NC_CLOBBER|NC_64BIT_OFFSET, info, &ncid);

    status = ncmpi_def_dim(ncid, "nparam", (long)nparam, &dimid1);
    status = ncmpi_def_dim(ncid, "ntracke", (long)ntracke, &dimid2);
    status = ncmpi_def_dim(ncid, "ntracki", (long)ntracki, &dimid3);

    particles_dim [0] =  dimid1;
    particles_dim [1] =  dimid2;
    status = ncmpi_def_var (ncid, "ptrackede", NC_DOUBLE, 2, particles_dim, &particles1_id);

    particles_dim [0] =  dimid1;
    particles_dim [1] =  dimid3;
    status = ncmpi_def_var (ncid, "ptrackedi", NC_DOUBLE, 2, particles_dim, &particles2_id);
    status = ncmpi_enddef(ncid);
    
    particles_start [0] = 0;
    particles_start [1] = offset_e;
    particles_count [0] = nparam;
    particles_count [1] = l_ntracke;
//printf ("%03d locale %4d globale %4d offsete %4d locali %4d globali %4d offseti %4d\n", rank, l_ntracke / 100000, ntracke / 100000, offset_e / 100000, l_ntracki / 100000, ntracki / 100000, offset_i / 100000);
    status = ncmpi_put_vara_double_all (ncid, particles1_id, particles_start, particles_count, ddata_e);

    particles_start [0] = 0;
    particles_start [1] = offset_i;
    particles_count [0] = nparam;
    particles_count [1] = l_ntracki;
    status = ncmpi_put_vara_double_all (ncid, particles2_id, particles_start, particles_count, ddata_i);
  
    status = ncmpi_close (ncid);
  
    status = MPI_Barrier (MPI_COMM_WORLD);

    end_time = MPI_Wtime ();
    t_time = end_time - start_time;

    sz = ((double) ntracki + ntracke) * nparam * sizeof (double) / (1024.0 * 1024.0 * 1024.0);

    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %lf %lf %lf \n", filename, size, sz, t_time, gps);

    free (ddata_e);
    free (ddata_i);
    MPI_Info_free (&info);
    MPI_Finalize ();

    return 0;
}
