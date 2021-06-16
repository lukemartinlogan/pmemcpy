module read_3d_data
! read patterns:
! 1. all vars (use restart)
! 2. all of 1 var
! 3. all of a few vars (3 for 3-d, for example)
! 4. 1 plane in each dimension for 1 variable
! 5. an arbitrary rectangular area (full dimensions)
! 6. an arbitrary area on an orthogonal plane (decomposition dimensions)

public :: read_pattern_1, read_pattern_2, read_pattern_3, read_pattern_4, read_pattern_5, read_pattern_6

contains

! 1. all vars (use restart)
subroutine read_pattern_1
    implicit none

    write (*,*) 'not implemented'
end subroutine

! 2. all of 1 var
subroutine read_pattern_2
    use netcdf
    implicit none
#include "mpif.h"

    integer :: nc_err
    integer :: ncid
    character(len=256) :: filename, nx_str, ny_str, nz_str
    character(len=100) :: dim_name
    integer, dimension(10) :: start
    integer, dimension(10) :: readsize

    integer :: nx_dimid, ny_dimid, nz_dimid

    integer :: grav_x_c_varid

    integer :: rank
    integer :: size
    integer :: nproc_x
    integer :: nproc_y
    integer :: nproc_z
    integer :: nx
    integer :: ny
    integer :: nz
    integer :: my_x_dim  ! size of local x
    integer :: my_y_dim  ! size of local y
    integer :: my_z_dim  ! size of local z
    integer :: x_min     ! offset for local x
    integer :: y_min     ! offset for local y
    integer :: z_min     ! offset for local z

    integer,dimension(10) :: dims, dim_ids
    integer :: dim_val

    real*8, dimension (:,:,:), allocatable :: grav_x_c

    real*8 :: start_time, end_time

    call getarg (2, filename)
    call getarg (3, nx_str)
    call getarg (4, ny_str)
    call getarg (5, nz_str)
    read (nx_str, '(i6)') nproc_x
    read (ny_str, '(i6)') nproc_y
    read (nz_str, '(i6)') nproc_z
    filename = trim(filename)//'.nc4'

    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_open_par (filename, nf90_nowrite, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)

    nc_err = nf90_inq_dimid (ncid, 'nx', nx_dimid)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nx_dimid, dim_name, nx)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'ny', ny_dimid)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, ny_dimid, dim_name, ny)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'nz', nz_dimid)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nz_dimid, dim_name, nz)
    if (nc_err /= nf90_noerr) write (*,'i3 a76') rank, nf90_strerror (nc_err)

    !if (rank == 0) write (*, 'a3 i3 a3 i3 a3 i3') 'nx', nx, 'ny', ny, 'nz', nz

    my_x_dim = nx / nproc_x
    my_y_dim = ny / nproc_y
    my_z_dim = nz / nproc_z
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim

    start(3) = x_min + 1
    start(2) = y_min + 1
    start(1) = z_min + 1

    readsize(3) = my_x_dim
    readsize(2) = my_y_dim
    readsize(1) = my_z_dim

!write (*,'i2 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,'i3 a3 a76') rank, ' A ', nf90_strerror (nc_err)

    nc_err = nf90_inquire_variable (ncid, grav_x_c_varid, dimids = dim_ids)
    if (nc_err /= nf90_noerr) write (*,'i3 a3 a76') rank, ' B ', nf90_strerror (nc_err)

    !if (rank == 0) nc_err = nf90_inquire_dimension (ncid, dim_ids(1), dim_name, dims(1))
    !if (rank == 0) nc_err = nf90_inquire_dimension (ncid, dim_ids(2), dim_name, dims(2))
    !if (rank == 0) nc_err = nf90_inquire_dimension (ncid, dim_ids(3), dim_name, dims(3))
    !if (rank == 0) write (*,'a9 i3 a9 i3 a9 i3') 'dims(1)', dims (1), 'dims(2)', dims(2), 'dims(3)', dims(3)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,'i3 a3 a76') rank, ' C ', nf90_strerror (nc_err)

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,'i3 a3 a76') rank, ' D ', nf90_strerror (nc_err)
    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 2, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
    call MPI_Finalize (nc_err)
end subroutine

! 3. all of a few vars (3 for 3-d, for example)
subroutine read_pattern_3
    use netcdf
    implicit none
#include "mpif.h"

    integer :: nc_err
    integer :: ncid
    character(len=256) :: filename, nx_str, ny_str, nz_str
    character(len=100) :: dim_name
    integer, dimension(10) :: start
    integer, dimension(10) :: readsize

    integer :: nx_dimid, ny_dimid, nz_dimid

    integer :: grav_x_c_varid, grav_y_c_varid, grav_z_c_varid

    integer :: rank
    integer :: size
    integer :: nproc_x
    integer :: nproc_y
    integer :: nproc_z
    integer :: nx
    integer :: ny
    integer :: nz
    integer :: my_x_dim  ! size of local x
    integer :: my_y_dim  ! size of local y
    integer :: my_z_dim  ! size of local z
    integer :: x_min     ! offset for local x
    integer :: y_min     ! offset for local y
    integer :: z_min     ! offset for local z

    real*8, dimension (:,:,:), allocatable :: grav_x_c
    real*8, dimension (:,:,:), allocatable :: grav_y_c
    real*8, dimension (:,:,:), allocatable :: grav_z_c

    real*8 :: start_time, end_time

    call getarg (2, filename)
    call getarg (3, nx_str)
    call getarg (4, ny_str)
    call getarg (5, nz_str)
    read (nx_str, '(i6)') nproc_x
    read (ny_str, '(i6)') nproc_y
    read (nz_str, '(i6)') nproc_z
    filename = trim(filename)//'.nc4'

    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_open_par (filename, nf90_nowrite, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_inq_dimid (ncid, 'nx', nx_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nx_dimid, dim_name, nx)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'ny', ny_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, ny_dimid, dim_name, ny)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'nz', nz_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nz_dimid, dim_name, nz)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    my_x_dim = nx / nproc_x
    my_y_dim = ny / nproc_y
    my_z_dim = nz / nproc_z
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim

    start(3) = x_min + 1
    start(2) = y_min + 1
    start(1) = z_min + 1

    readsize(3) = my_x_dim
    readsize(2) = my_y_dim
    readsize(1) = my_z_dim

!write (*,'i2 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))
    allocate (grav_y_c (readsize (1), readsize(2), readsize(3)))
    allocate (grav_z_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_varid (ncid, 'E', grav_y_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_varid (ncid, 'F', grav_z_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_get_var (ncid, grav_y_c_varid, grav_y_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_get_var (ncid, grav_z_c_varid, grav_z_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 3, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
    call MPI_Finalize (nc_err)
end subroutine

! 4. 1 plane in each dimension for 1 variable
subroutine read_pattern_4
    use netcdf
    implicit none
#include "mpif.h"

    integer :: nc_err
    integer :: ncid
    character(len=256) :: filename, nx_str, ny_str, nz_str
    character(len=100) :: dim_name

    integer :: nx_dimid, ny_dimid, nz_dimid

    integer :: grav_x_c_varid
    integer, dimension(10) :: start
    integer, dimension(10) :: readsize

    integer :: rank
    integer :: size
    integer :: nproc_x
    integer :: nproc_y
    integer :: nproc_z
    integer :: nx
    integer :: ny
    integer :: nz
    integer :: my_x_dim  ! size of local x
    integer :: my_y_dim  ! size of local y
    integer :: my_z_dim  ! size of local z
    integer :: x_min     ! offset for local x
    integer :: y_min     ! offset for local y
    integer :: z_min     ! offset for local z

    real*8, dimension (:,:,:), allocatable :: grav_x_c

    real*8 :: start_time, end_time

    call getarg (2, filename)
    call getarg (3, nx_str)
    call getarg (4, ny_str)
    call getarg (5, nz_str)
    read (nx_str, '(i6)') nproc_x
    read (ny_str, '(i6)') nproc_y
    read (nz_str, '(i6)') nproc_z
    filename = trim(filename)//'.nc4'

    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_open_par (filename, nf90_nowrite, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_inq_dimid (ncid, 'nx', nx_dimid)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nx_dimid, dim_name, nx)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'ny', ny_dimid)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, ny_dimid, dim_name, ny)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'nz', nz_dimid)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nz_dimid, dim_name, nz)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(3)
    my_x_dim = ny / nproc_x
    my_y_dim = nx / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = nz / 2 + 1
    start(2) = x_min + 1
    start(3) = y_min + 1

    readsize(1) = 1
    readsize(2) = my_x_dim
    readsize(3) = my_y_dim

!write (*,'i2 i5 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 1, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(2)
    my_x_dim = nz / nproc_x
    my_y_dim = nx / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = x_min + 1
    start(2) = ny / 2 + 1
    start(3) = y_min + 1

    readsize(1) = my_x_dim
    readsize(2) = 1
    readsize(3) = my_y_dim

!write (*,'i2 i5 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 2, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(1)
    my_x_dim = nz / nproc_x
    my_y_dim = ny / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = x_min + 1
    start(2) = y_min + 1
    start(3) = nx / 2 + 1

    readsize(1) = my_x_dim
    readsize(2) = my_y_dim
    readsize(3) = 1

!write (*,'i2 i5 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 3, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,'i5 a73') rank, nf90_strerror (nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 f') filename, 4, size, nproc_x, nproc_y, end_time - start_time
    endif
    call MPI_Finalize (nc_err)
end subroutine

! 5. an arbitrary rectangular area (full dimensions)
subroutine read_pattern_5
    use netcdf
    implicit none
#include "mpif.h"

    integer :: nc_err
    integer :: ncid
    character(len=256) :: filename, nx_str, ny_str, nz_str
    character(len=100) :: dim_name

    integer :: nx_dimid, ny_dimid, nz_dimid

    integer :: grav_x_c_varid
    integer, dimension(10) :: start
    integer, dimension(10) :: readsize

    integer :: rank
    integer :: size
    integer :: nproc_x
    integer :: nproc_y
    integer :: nproc_z
    integer :: nx
    integer :: ny
    integer :: nz
    integer :: my_x_dim  ! size of local x
    integer :: my_y_dim  ! size of local y
    integer :: my_z_dim  ! size of local z
    integer :: x_min     ! offset for local x
    integer :: y_min     ! offset for local y
    integer :: z_min     ! offset for local z

    real*8, dimension (:,:,:), allocatable :: grav_x_c

    real*8 :: start_time, end_time

    call getarg (2, filename)
    call getarg (3, nx_str)
    call getarg (4, ny_str)
    call getarg (5, nz_str)
    read (nx_str, '(i6)') nproc_x
    read (ny_str, '(i6)') nproc_y
    read (nz_str, '(i6)') nproc_z
    filename = trim(filename)//'.nc4'

    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_open_par (filename, nf90_nowrite, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_inq_dimid (ncid, 'nx', nx_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nx_dimid, dim_name, nx)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'ny', ny_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, ny_dimid, dim_name, ny)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'nz', nz_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nz_dimid, dim_name, nz)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    my_x_dim = nx / nproc_x / 2 
    my_y_dim = ny / nproc_y / 2
    my_z_dim = nz / nproc_z / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim 

    start(3) = nx / 4 + x_min + 1
    start(2) = ny / 4 + y_min + 1
    start(1) = nz / 4 + z_min + 1
    
    readsize(3) = my_x_dim
    readsize(2) = my_y_dim
    readsize(1) = my_z_dim

!write (*,'i2 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
!    if (rank == 0) then
!    write (*,*) 'dims(1) = ', dims (1)
!    write (*,*) 'dims(2) = ', dims (2)
!    write (*,*) 'dims(3) = ', dims (3)
!    endif

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 5, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
    call MPI_Finalize (nc_err)
end subroutine

! 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
subroutine read_pattern_6
    use netcdf
    implicit none
#include "mpif.h"

    integer :: nc_err
    integer :: ncid
    character(len=256) :: filename, nx_str, ny_str, nz_str
    character(len=100) :: dim_name

    integer :: nx_dimid, ny_dimid, nz_dimid

    integer :: grav_x_c_varid
    integer, dimension(10) :: start
    integer, dimension(10) :: readsize

    integer :: rank
    integer :: size
    integer :: nproc_x
    integer :: nproc_y
    integer :: nproc_z
    integer :: nx
    integer :: ny
    integer :: nz
    integer :: my_x_dim  ! size of local x
    integer :: my_y_dim  ! size of local y
    integer :: my_z_dim  ! size of local z
    integer :: x_min     ! offset for local x
    integer :: y_min     ! offset for local y
    integer :: z_min     ! offset for local z

    real*8, dimension (:,:,:), allocatable :: grav_x_c

    real*8 :: start_time, end_time

    call getarg (2, filename)
    call getarg (3, nx_str)
    call getarg (4, ny_str)
    read (nx_str, '(i6)') nproc_x
    read (ny_str, '(i6)') nproc_y
    filename = trim(filename)//'.nc4'

    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_open_par (filename, nf90_nowrite, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_inq_dimid (ncid, 'nx', nx_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nx_dimid, dim_name, nx)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'ny', ny_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, ny_dimid, dim_name, ny)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inq_dimid (ncid, 'nz', nz_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
    nc_err = nf90_inquire_dimension (ncid, nz_dimid, dim_name, nz)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! read in dim (1)
    my_x_dim = ny / nproc_x / 2
    my_y_dim = nx / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim
    
    start(1) = nz / 4 + 1
    start(2) = ny / 4 + y_min + 1
    start(3) = nx / 4 + x_min + 1
    
    readsize(1) = 1
    readsize(2) = my_x_dim
    readsize(3) = my_y_dim

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 1, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! read in dim (2)
    my_x_dim = nz / nproc_x / 2
    my_y_dim = nx / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim
    
    start(1) = nz / 4 + x_min + 1
    start(2) = ny / 4 + 1
    start(3) = nx / 4 + y_min + 1
    
    readsize(1) = my_x_dim
    readsize(2) = 1
    readsize(3) = my_y_dim

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 2, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! read in dim (3)
    my_x_dim = nz / nproc_x / 2
    my_y_dim = ny / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim
    
    start(1) = nz / 4 + x_min + 1
    start(2) = ny / 4 + y_min + 1
    start(3) = nx / 4 + 1
    
    readsize(1) = my_x_dim
    readsize(2) = my_y_dim
    readsize(3) = 1

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 3, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    nc_err = nf90_inq_varid (ncid, 'D', grav_x_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    nc_err = nf90_get_var (ncid, grav_x_c_varid, grav_x_c, start, readsize)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, nf90_strerror (nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 f') filename, 6, size, nproc_x, nproc_y, end_time - start_time
    endif
    call MPI_Finalize (nc_err)
end subroutine

end module

program read_3d
    use read_3d_data
    implicit none

    character(len=100) :: pattern_str

    call getarg (1, pattern_str)

! 1. all vars (use restart)
! 2. all of 1 var
! 3. all of a few vars (3 for 3-d, for example)
! 4. 1 plane in each dimension for 1 variable
! 5. an arbitrary rectangular area (full dimensions)
! 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
    select case (pattern_str)
        case ("1")
            call read_pattern_1 ()
        case ("2")
            call read_pattern_2 ()
        case ("3")
            call read_pattern_3 ()
        case ("4")
            call read_pattern_4 ()
        case ("5")
            call read_pattern_5 ()
        case ("6")
            call read_pattern_6 ()
    end select

end program
