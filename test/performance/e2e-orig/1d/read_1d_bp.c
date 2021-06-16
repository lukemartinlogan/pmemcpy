// read patterns:
// 1. all vars (use restart)
// 2. all of 1 var
// 3. all of a few vars (3 for 3-d, for example)
// 4. 1 plane in each dimension for 1 variable
// 5. an arbitrary rectangular area (full dimensions)
// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include <adios_read.h>

// 1. all vars (use restart)
int read_pattern_1 (int argc, char ** argv)
{
    printf ("not implemented\n");
}

// 2. all of 1 var
int read_pattern_2 (int argc, char ** argv)
{
    int adios_err;
    char filename [256];
    uint64_t start [10];
    uint64_t readsize [10];

    ADIOS_FILE * adios_handle;
    ADIOS_GROUP * group_handle;
    ADIOS_VARINFO * var_info;
    uint64_t read_bytes;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int nparam;
    int64_t ntracke, ntracki;

    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.bp", argv [2]);

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_handle = adios_fopen (filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg ()); 

    group_handle = adios_gopen (adios_handle, "particles");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg ()); 

    read_bytes = adios_read_var (group_handle, "/nparam", start, readsize, &nparam);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg ()); 
    read_bytes = adios_read_var (group_handle, "/ntracke", start, readsize, &ntracke);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg ()); 
    read_bytes = adios_read_var (group_handle, "/ntracki", start, readsize, &ntracki);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg ()); 

    //if (rank == 0)
    //    printf ("nparam %lld ntracke %lld ntracki %lld\n", nparam, ntracke, ntracki);

    my_x_dim = nparam;
    my_y_dim = ntracke / size;
    x_min = 0;
    y_min = rank * my_y_dim;

    start [0] = x_min;
    start [1] = y_min;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

//printf ("%2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

    var_info = adios_inq_var (group_handle, "/ptrackede");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ()); 

    read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ()); 

    adios_err = adios_gclose (group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ()); 
    adios_err = adios_fclose (adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ()); 

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 2, size, end_time - start_time);

    adios_err = MPI_Finalize ();
}

// 3. all of a few vars (3 for 3-d, for example)
int read_pattern_3 (int argc, char ** argv)
{
    int adios_err;
    char filename [256];
    uint64_t start [10];
    uint64_t readsize [10];

    ADIOS_FILE * adios_handle;
    ADIOS_GROUP * group_handle;
    ADIOS_VARINFO * var_info, * var_info2;
    uint64_t read_bytes;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int nparam;
    int64_t ntracke;
    int64_t ntracki;
    double * grav_x_c;
    double * grav_y_c;

    double start_time, end_time;

    sprintf (filename, "%s.bp", argv [2]);

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_handle = adios_fopen (filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg ());

    group_handle = adios_gopen (adios_handle, "particles");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/nparam", start, readsize, &nparam);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracke", start, readsize, &ntracke);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracki", start, readsize, &ntracki);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg ());

    //if (rank == 0)
    //    printf ("nparam %lld ntracke %lld ntracki %lld\n", nparam, ntracke, ntracki);

    my_x_dim = nparam;
    my_y_dim = ntracke / size;
    x_min = 0;
    y_min = rank * my_y_dim;

    start [0] = x_min;
    start [1] = y_min;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

//printf ("%2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);
    grav_y_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

    var_info = adios_inq_var (group_handle, "/ptrackede");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ());

    var_info2 = adios_inq_var (group_handle, "/ptrackedi");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/ptrackedi", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());

    adios_err = adios_gclose (group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());
    adios_err = adios_fclose (adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 3, size, end_time - start_time);

    adios_err = MPI_Finalize ();
}

// 4. 1 plane in each dimension for 1 variable
int read_pattern_4 (int argc, char ** argv)
{
    int adios_err;
    char filename [256];
    char dim_name [100];
    uint64_t start [10];
    uint64_t readsize [10];

    ADIOS_FILE * adios_handle;
    ADIOS_GROUP * group_handle;
    ADIOS_VARINFO * var_info;
    uint64_t read_bytes;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int nparam;
    int64_t ntracke;
    int64_t ntracki;
    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.bp", argv [2]);

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_handle = adios_fopen (filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg ());

    group_handle = adios_gopen (adios_handle, "particles");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/nparam", start, readsize, &nparam);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracke", start, readsize, &ntracke);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracki", start, readsize, &ntracki);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg ());

    //if (rank == 0)
    //    printf ("nparam %lld ntracke %lld ntracki %lld\n", nparam, ntracke, ntracki);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(1)
    my_x_dim = 1;
    my_y_dim = ntracke / size;
    x_min = nparam / 2;
    y_min = rank * my_y_dim;

    start [0] = x_min;
    start [1] = y_min;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

//printf ("1 %2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

    var_info = adios_inq_var (group_handle, "/ptrackede");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! do a plane in dim(2)
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

    if (rank == 0)
    {
//printf ("2 %2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

        read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
        if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());
    }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    adios_err = adios_gclose (group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());
    adios_err = adios_fclose (adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 4, size, end_time - start_time);

    adios_err = MPI_Finalize ();
}

// 5. an arbitrary rectangular area (full dimensions)
int read_pattern_5 (int argc, char ** argv)
{
    int adios_err;
    char filename [256];
    char dim_name [100];
    uint64_t start [10];
    uint64_t readsize [10];

    ADIOS_FILE * adios_handle;
    ADIOS_GROUP * group_handle;
    ADIOS_VARINFO * var_info;
    uint64_t read_bytes;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int nparam;
    int64_t ntracke;
    int64_t ntracki;
    double * grav_x_c = 0;

    double start_time, end_time;

    sprintf (filename, "%s.bp", argv [2]);

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_handle = adios_fopen (filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg ());

    group_handle = adios_gopen (adios_handle, "particles");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/nparam", start, readsize, &nparam);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracke", start, readsize, &ntracke);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracki", start, readsize, &ntracki);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg ());

    //if (rank == 0)
    //    printf ("nparam %lld ntracke %lld ntracki %lld\n", nparam, ntracke, ntracki);

    my_x_dim = nparam / 2;
    my_y_dim = ntracke / size / 2;
    x_min = nparam / 4;
    y_min = rank * my_y_dim;

    start [0] = x_min;
    start [1] = ntracke / 4 + y_min;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

//printf ("%2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

   var_info = adios_inq_var (group_handle, "/ptrackede");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());

    adios_err = adios_gclose (group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());
    adios_err = adios_fclose (adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 5, size, end_time - start_time);

    adios_err = MPI_Finalize ();
}

// 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
int read_pattern_6 (int argc, char ** argv)
{
    int adios_err;
    char filename [256];
    char dim_name [100];
    size_t start [10];
    size_t readsize [10];

    ADIOS_FILE * adios_handle;
    ADIOS_GROUP * group_handle;
    ADIOS_VARINFO * var_info;
    uint64_t read_bytes;

    int rank;
    int size;
    int my_x_dim;    // size of local x
    int my_y_dim;    // size of local y
    int x_min;       // offset for local x
    int y_min;       // offset for local y

    int nparam;
    int64_t ntracke;
    int64_t ntracki;
    double * grav_x_c;

    double start_time, end_time;

    sprintf (filename, "%s.bp", argv [2]);

    adios_err = MPI_Init (&argc, &argv);
    adios_err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    adios_err = MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    adios_handle = adios_fopen (filename, MPI_COMM_WORLD);
    if (!adios_handle) fprintf(stderr, "%d: %s (1)\n", rank, adios_errmsg ());

    group_handle = adios_gopen (adios_handle, "particles");
    if (!group_handle) fprintf(stderr, "%d: %s (1a)\n", rank, adios_errmsg ());

    read_bytes = adios_read_var (group_handle, "/nparam", start, readsize, &nparam);
    if (!read_bytes) fprintf(stderr, "%d: %s (2)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracke", start, readsize, &ntracke);
    if (!read_bytes) fprintf(stderr, "%d: %s (4)\n", rank, adios_errmsg ());
    read_bytes = adios_read_var (group_handle, "/ntracki", start, readsize, &ntracki);
    if (!read_bytes) fprintf(stderr, "%d: %s (6)\n", rank, adios_errmsg ());

    //if (rank == 0)
    //    printf ("nparam %lld ntracke %lld ntracki %lld\n", nparam, ntracke, ntracki);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // read in dim(1)
    my_x_dim = nparam / 2;
    my_y_dim = 1;
    x_min = nparam / 4;
    y_min = ntracke / 2;

    start [0] = x_min;
    start [1] = y_min;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

    var_info = adios_inq_var (group_handle, "/ptrackede");
    if (!var_info) fprintf(stderr, "%d: %s (8)\n", rank, adios_errmsg ());

    if (rank == 0)
    {
//        printf ("1 %2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);
        read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
        if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());
    }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // read in dim(2)
    my_x_dim = 1;
    my_y_dim = ntracke / size / 2;
    x_min = nparam / 2;
    y_min = ntracke / 4;

    start [0] = x_min;
    start [1] = y_min + rank * my_y_dim;

    readsize [0] = my_x_dim;
    readsize [1] = my_y_dim;

//printf ("2 %2d start[0] %3d start[1] %3d size[0] %3d size[1] %3d\n", rank, start [0], start [1], readsize [0], readsize [1]);

    free (grav_x_c);
    grav_x_c = malloc (sizeof (double) * readsize [0] * readsize [1]);

    read_bytes = adios_read_var (group_handle, "/ptrackede", start, readsize, grav_x_c);
    if (!read_bytes) fprintf(stderr, "%d: %s (A)\n", rank, adios_errmsg ());
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    adios_err = adios_gclose (group_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());
    adios_err = adios_fclose (adios_handle);
    if (adios_err) fprintf(stderr, "%d: %s (B)\n", rank, adios_errmsg ());

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    if (rank == 0)
        printf ("%s %d %d %lf\n", filename, 6, size, end_time - start_time);

    adios_err = MPI_Finalize ();
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
