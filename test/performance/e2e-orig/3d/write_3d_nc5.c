/***********************************************************
 *
 * This test program writes a netCDF file using the parallel
 * netCDF library using MPI-IO. 
 *
 **********************************************************/
/*
cc test_write_alloc3d.c  -o genarray_pnc -I/ccs/home/sklasky/parallel-netcdf/jaguar/include -L/ccs/home/sklasky/parallel-netcdf/jaguar/lib -lpnetcdf
*/



#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnetcdf.h>
#include <string.h>

/* Prototype for functions used only in this file */
static void handle_error(int status);

static void handle_error(int status) {
  fprintf(stderr, "%s\n", ncmpi_strerror(status));
}

int main(int argc, char **argv) {

  int i, j, k,l;
  int status;
  int ncid;
  int dimid1, dimid2, dimid3, udimid;
  int cube_dim[3] ;
  int  npx, npy, npz, ndx, ndy, ndz, nx, ny, nz;
  int  offx, offy, offz, posx, posy, posz;
  int  offsets[3], lsize[3], gsize[3];
  MPI_Offset cube_start[3];
  MPI_Offset cube_count[3];
  int cube1_id,cube2_id,cube3_id,cube4_id,cube5_id;
  int cube6_id,cube7_id,cube8_id,cube9_id,cube10_id;
  static char title[] = "example netCDF dataset";
  static char description[] = "3-D integer array";
  double *ddata=NULL;
  int rank;
  int nprocs;
  MPI_Comm comm = MPI_COMM_WORLD;
  char oname[128];
  double start_time, end_time, t_time,sz, gps;
  MPI_Info info;
  MPI_File fh;
  int time;
    char filename [256];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Info_create(&info);
  //MPI_Info_set(info, "romio_cb_write", "disable");
  MPI_Info_set(info, "romio_ds_write", "disable");

    sprintf (filename, "%s.nc5", argv [1]);

  //  strcat(oname,argv[1]);
  npx = atoi(argv[2]);
  npy = atoi(argv[3]);
  npz = atoi(argv[4]);

  ndx = atoi(argv[5]);
  ndy = atoi(argv[6]);
  ndz = atoi(argv[7]);
  
  nx = npx * ndx;
  ny = npy * ndy;
  nz = npz * ndz;
  
  posx = rank%npx;
  posy = (rank/npx) %npy;
  posz = rank/(npx*npy);
  
  offx = posx * ndx;
  offy = posy * ndy;
  offz = posz * ndz;

  ddata = (double *)malloc(sizeof(double*)*ndx*ndy*ndz);

  for (i=0;i<ndx*ndy*ndz;i++)
  {
	ddata [i] = rank;
  } 
  
  //if (rank == 0) fprintf(stderr, "Test... %s %d %d\n",filename,l,ndx*ndy*ndz);
  
  cube_start[0] = offx;
  cube_start[1] = offy;
  cube_start[2] = offz;
 
  cube_count[0] = ndx;
  cube_count[1] = ndy;
  cube_count[2] = ndz;
// I will need the number of procs divisible by 8

//printf ("%03d start(0) %3d start(1) %3d start(2) %3d size(0) %3d size(1) %3d size(2) %3d\n", rank, cube_start[0], cube_start[1], cube_start[2], cube_count[0], cube_count[1], cube_count[2]);

// start the timing now.
  
     status = MPI_Barrier(MPI_COMM_WORLD);
     start_time = MPI_Wtime();
   
     status = ncmpi_create(comm, filename, NC_CLOBBER|NC_64BIT_OFFSET, info, &ncid);

   
   
     status = ncmpi_put_att_text (ncid, NC_GLOBAL, "title",
                             strlen(title), title);
   
     status = ncmpi_def_dim(ncid, "nx", (long)nx, &dimid1);
     status = ncmpi_def_dim(ncid, "ny", (long)ny, &dimid2);
     status = ncmpi_def_dim(ncid, "nz", (long)nz, &dimid3);
     cube_dim[0] =  dimid1;
     cube_dim[1] =  dimid2;
     cube_dim[2] =  dimid3;
   
     status = ncmpi_def_var (ncid, "A", NC_DOUBLE, 3, cube_dim, &cube1_id);
     status = ncmpi_def_var (ncid, "B", NC_DOUBLE, 3, cube_dim, &cube2_id);
     status = ncmpi_def_var (ncid, "C", NC_DOUBLE, 3, cube_dim, &cube3_id);
     status = ncmpi_def_var (ncid, "D", NC_DOUBLE, 3, cube_dim, &cube4_id);
     status = ncmpi_def_var (ncid, "E", NC_DOUBLE, 3, cube_dim, &cube5_id);
     status = ncmpi_def_var (ncid, "F", NC_DOUBLE, 3, cube_dim, &cube6_id);
     status = ncmpi_def_var (ncid, "G", NC_DOUBLE, 3, cube_dim, &cube7_id);
     status = ncmpi_def_var (ncid, "H", NC_DOUBLE, 3, cube_dim, &cube8_id);
     status = ncmpi_def_var (ncid, "I", NC_DOUBLE, 3, cube_dim, &cube9_id);
     status = ncmpi_def_var (ncid, "J", NC_DOUBLE, 3, cube_dim, &cube10_id);
     status = ncmpi_enddef(ncid);
     
    
     status = ncmpi_put_vara_double_all(ncid, cube1_id,
				       cube_start, cube_count,
				    (const void *)&(ddata[0]));
     status = ncmpi_put_vara_double_all(ncid, cube2_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube3_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube4_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube5_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube6_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube7_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube8_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube9_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
  
     status = ncmpi_put_vara_double_all(ncid, cube10_id,
				    cube_start, cube_count,
				    (const void *)&(ddata[0]));
     status = ncmpi_close(ncid);
  
    status = MPI_Barrier (MPI_COMM_WORLD);

    end_time = MPI_Wtime ();
    t_time = end_time - start_time;

    sz = (8.0 * 8.0 * nx * ny * ((double) nz)) / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %d %d %d %lf %lf %lf\n", filename, nprocs, ndx, ndy, ndz, sz, t_time, gps);
    free (ddata);
    MPI_Info_free (&info);
    MPI_Finalize ();

    return 0;
}

