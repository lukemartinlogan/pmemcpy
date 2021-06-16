#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mpi.h>
#include <adios.h>

int main (int argc, char ** argv)
{
    int rank, size;

    char filename [256];
    int64_t adios_handle;
    uint64_t adios_groupsize, adios_totalsize, adios_err;

    int32_t mype, nparam, l_ntracke, l_ntracki;
    int64_t ntracke, ntracki, offset_e, offset_i;

    double * ptrackede, * ptrackedi;

    int average_particle_count;
    float average_variation;

    MPI_Comm comm = MPI_COMM_WORLD;

    double start_time, end_time, t_time, sz, gps;

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (comm, &rank);
    adios_err = MPI_Comm_size (comm, &size);

    adios_err = adios_init ("1d.xml");

    mype = rank;
    nparam = 8;                // 8 components per tracked particle
    average_particle_count = 1000000; // 1 million per process on average
    average_variation = 0.2;          // amount to vary the count per proc

    ntracki = average_particle_count * size;
    ntracke = average_particle_count * size;

    // assume a 20% +/- variance in the number of particles per process
    // to represent the non-even distribution that changes over time
    if ((rank % 2) == 0) // even proc id
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


    ptrackede = (double *) malloc (sizeof (double) * nparam * l_ntracke);
    ptrackedi = (double *) malloc (sizeof (double) * nparam * l_ntracki);

    double * t = ptrackede;
    for (int i = 0; i < nparam * l_ntracke; i++)
        *t++ = rank;

    t = ptrackedi;
    for (int i = 0; i < nparam * l_ntracki; i++)
        *t++ = rank;

///////////////////////////////////

//printf ("%03d locale %4d globale %4d offsete %4d locali %4d globali %4d offseti %4d\n", rank, l_ntracke / 100000, ntracke / 100000, offset_e / 100000, l_ntracki / 100000, ntracki / 100000, offset_i / 100000);

    sprintf (filename, "%s.bp", argv [1]);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_err = adios_open (&adios_handle, "particles", filename, "w", &comm);
#include "gwrite_particles.ch"
    adios_err = adios_close (adios_handle);

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    t_time = end_time - start_time;
    sz = ((double) ntracki + ntracke) * nparam * sizeof (double) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %lf %lf %lf\n", filename, size, sz, t_time, gps);

    adios_err = adios_finalize (rank);

    adios_err = MPI_Finalize ();

    return 0;
}
