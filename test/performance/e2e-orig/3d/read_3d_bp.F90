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
    implicit none
#include "mpif.h"

    integer :: adios_err
    integer*8 :: adios_handle
    integer*8 :: group_handle
    integer*8 :: read_bytes
    character(len=256) :: filename, nx_str, ny_str, nz_str
    integer :: group_count
    integer :: var_count
    integer :: attr_count
    integer :: time_start
    integer :: time_steps
    integer :: var_type
    integer :: vtimed
    integer :: var_rank
    integer*8, dimension(10) :: dims
    integer*8, dimension(10) :: start
    integer*8, dimension(10) :: readsize
    character (len=100), dimension(5000) :: groupname_list
    character (len=100), dimension(5000) :: varname_list
    character (len=100), dimension(5000) :: attrname_list

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
    filename = trim(filename)//'.bp'

    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_fopen (adios_handle, filename, MPI_COMM_WORLD, group_count, adios_err)

    call adios_gopen (adios_handle, group_handle, 'data', var_count, attr_count, adios_err)

    call adios_read_var (group_handle, '/nx', start, readsize, nx, read_bytes)
    call adios_read_var (group_handle, '/ny', start, readsize, ny, read_bytes)
    call adios_read_var (group_handle, '/nz', start, readsize, nz, read_bytes)
    my_x_dim = nx / nproc_x
    my_y_dim = ny / nproc_y
    my_z_dim = nz / nproc_z
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim

    start(3) = x_min
    start(2) = y_min
    start(1) = z_min

    readsize(3) = my_x_dim
    readsize(2) = my_y_dim
    readsize(1) = my_z_dim

!write (*, 'i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') rank, 'start(1)', start(1), 'start(2)', start(2), 'start(3)', start(3), 'size(1)', readsize(1), 'size(2)', readsize(2), 'size(3)', readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_inq_var (group_handle, '/D', var_type, var_rank, dims, vtimed, adios_err)

     !if (rank == 0) write (*,'a9 i3 a9 i3 a9 i3') 'dims(1)', dims (1), 'dims(2)', dims(2), 'dims(3)', dims(3)

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)

    call adios_gclose (group_handle, adios_err)

    call adios_fclose (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    call MPI_Finalize (adios_err)
    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 2, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
end subroutine

! 3. all of a few vars (3 for 3-d, for example)
subroutine read_pattern_3
    implicit none
#include "mpif.h"

    integer :: adios_err
    integer*8 :: adios_handle
    integer*8 :: group_handle
    integer*8 :: read_bytes
    character(len=256) :: filename, nx_str, ny_str, nz_str
    integer :: group_count
    integer :: var_count
    integer :: attr_count
    integer :: time_start
    integer :: time_steps
    integer :: var_type
    integer :: vtimed
    integer :: var_rank
    integer*8, dimension(10) :: dims
    integer*8, dimension(10) :: start
    integer*8, dimension(10) :: readsize
    character (len=100), dimension(5000) :: groupname_list
    character (len=100), dimension(5000) :: varname_list
    character (len=100), dimension(5000) :: attrname_list

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
    filename = trim(filename)//'.bp'

    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_fopen (adios_handle, filename, MPI_COMM_WORLD, group_count, adios_err)

    call adios_gopen (adios_handle, group_handle, 'data', var_count, attr_count, adios_err)

    call adios_read_var (group_handle, '/nx', start, readsize, nx, read_bytes)
    call adios_read_var (group_handle, '/ny', start, readsize, ny, read_bytes)
    call adios_read_var (group_handle, '/nz', start, readsize, nz, read_bytes)
    my_x_dim = nx / nproc_x
    my_y_dim = ny / nproc_y
    my_z_dim = nz / nproc_z
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim

    start(3) = x_min
    start(2) = y_min
    start(1) = z_min

    readsize(3) = my_x_dim
    readsize(2) = my_y_dim
    readsize(1) = my_z_dim

!write (*, 'i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') rank, 'start(1)', start(1), 'start(2)', start(2), 'start(3)', start(3), 'size(1)', readsize(1), 'size(2)', readsize(2), 'size(3)', readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))
    allocate (grav_y_c (readsize (1), readsize(2), readsize(3)))
    allocate (grav_z_c (readsize (1), readsize(2), readsize(3)))

!    call adios_inq_var (group_handle, '/D', var_type, var_rank, dims, vtimed, adios_err)
!    if (rank == 0) then
!    write (*,*) 'dims(1) = ', dims (1)
!    write (*,*) 'dims(2) = ', dims (2)
!    write (*,*) 'dims(3) = ', dims (3)
!    endif

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
    call adios_read_var (group_handle, '/E', start, readsize, grav_y_c, read_bytes)
    call adios_read_var (group_handle, '/F', start, readsize, grav_z_c, read_bytes)

    call adios_gclose (group_handle, adios_err)

    call adios_fclose (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    call MPI_Finalize (adios_err)

    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 3, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
end subroutine

! 4. 1 plane in each dimension for 1 variable
subroutine read_pattern_4
    implicit none
#include "mpif.h"

    integer :: adios_err
    integer*8 :: adios_handle
    integer*8 :: group_handle
    integer*8 :: read_bytes
    character(len=256) :: filename, nx_str, ny_str, nz_str
    integer :: group_count
    integer :: var_count
    integer :: attr_count
    integer :: time_start
    integer :: time_steps
    integer :: var_type
    integer :: vtimed
    integer :: var_rank
    integer*8, dimension(10) :: dims
    integer*8, dimension(10) :: start
    integer*8, dimension(10) :: readsize
    character (len=100), dimension(5000) :: groupname_list
    character (len=100), dimension(5000) :: varname_list
    character (len=100), dimension(5000) :: attrname_list

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
    filename = trim(filename)//'.bp'

    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_fopen (adios_handle, filename, MPI_COMM_WORLD, group_count, adios_err)

    call adios_gopen (adios_handle, group_handle, 'data', var_count, attr_count, adios_err)

    call adios_read_var (group_handle, '/nx', start, readsize, nx, read_bytes)
    call adios_read_var (group_handle, '/ny', start, readsize, ny, read_bytes)
    call adios_read_var (group_handle, '/nz', start, readsize, nz, read_bytes)

    call adios_inq_var (group_handle, '/D', var_type, var_rank, dims, vtimed, adios_err)
    !if (rank == 0) write (*,'a9 i3 a9 i3 a9 i3') 'dims(1)', dims (1), 'dims(2)', dims(2), 'dims(3)', dims(3)
    !if (rank == 0) write (*, 'a3 i3 a3 i3 a3 i3 a9 i3 a9 i3') 'nx', nx, 'ny', ny, 'nz', nz, 'nproc_x', nproc_x, 'nproc_y', nproc_y

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(1)
    my_x_dim = ny / nproc_x
    my_y_dim = nx / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = nz / 2
    start(2) = x_min
    start(3) = y_min

    readsize(1) = 1
    readsize(2) = my_x_dim
    readsize(3) = my_y_dim

!write (*, 'i2 i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') 1, rank, 'start(1)', start(1), 'start(2)', start(2), 'start(3)', start(3), 'size(1)', readsize(1), 'size(2)', readsize(2), 'size(3)', readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(2)
    my_x_dim = nz / nproc_x
    my_y_dim = nx / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = x_min
    start(2) = ny / 2
    start(3) = y_min

    readsize(1) = my_x_dim
    readsize(2) = 1
    readsize(3) = my_y_dim

!write (*, 'i2 i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') 2, rank, 'start(1)', start(1), 'start(2)', start(2), 'start(3)', start(3), 'size(1)', readsize(1), 'size(2)', readsize(2), 'size(3)', readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a plane in dim(3)
    my_x_dim = nz / nproc_x
    my_y_dim = ny / nproc_y
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = x_min
    start(2) = y_min
    start(3) = nx / 2

    readsize(1) = my_x_dim
    readsize(2) = my_y_dim
    readsize(3) = 1

!write (*, 'i2 i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') 3, rank, 'start(1)', start(1), 'start(2)', start(2), 'start(3)', start(3), 'size(1)', readsize(1), 'size(2)', readsize(2), 'size(3)', readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    call adios_gclose (group_handle, adios_err)

    call adios_fclose (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    call MPI_Finalize (adios_err)
    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 f') filename, 4, size, nproc_x, nproc_y, end_time - start_time
    endif
end subroutine

! 5. an arbitrary rectangular area (full dimensions)
subroutine read_pattern_5
    implicit none
#include "mpif.h"

    integer :: adios_err
    integer*8 :: adios_handle
    integer*8 :: group_handle
    integer*8 :: read_bytes
    character(len=256) :: filename, nx_str, ny_str, nz_str
    integer :: group_count
    integer :: var_count
    integer :: attr_count
    integer :: time_start
    integer :: time_steps
    integer :: var_type
    integer :: vtimed
    integer :: var_rank
    integer*8, dimension(10) :: dims
    integer*8, dimension(10) :: start
    integer*8, dimension(10) :: readsize
    character (len=100), dimension(5000) :: groupname_list
    character (len=100), dimension(5000) :: varname_list
    character (len=100), dimension(5000) :: attrname_list

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
    filename = trim(filename)//'.bp'

    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_fopen (adios_handle, filename, MPI_COMM_WORLD, group_count, adios_err)

    call adios_gopen (adios_handle, group_handle, 'data', var_count, attr_count, adios_err)

    call adios_read_var (group_handle, '/nx', start, readsize, nx, read_bytes)
    call adios_read_var (group_handle, '/ny', start, readsize, ny, read_bytes)
    call adios_read_var (group_handle, '/nz', start, readsize, nz, read_bytes)

    call adios_inq_var (group_handle, '/D', var_type, var_rank, dims, vtimed, adios_err)
    !if (rank == 0) write (*,'a9 i3 a9 i3 a9 i3') 'dims(1)', dims (1), 'dims(2)', dims(2), 'dims(3)', dims(3)
    !if (rank == 0) write (*, 'a3 i3 a3 i3 a3 i3 a9 i3 a9 i3') 'nx', nx, 'ny', ny, 'nz', nz, 'nproc_x', nproc_x, 'nproc_y', nproc_y

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    my_x_dim = nx / nproc_x / 2
    my_y_dim = ny / nproc_y / 2
    my_z_dim = nz / nproc_z / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = MOD(rank/nproc_x, nproc_y) * my_y_dim
    z_min = rank/(nproc_x*nproc_y) * my_z_dim

    start(1) = nz / 4 + z_min
    start(2) = ny / 4 + y_min
    start(3) = nx / 4 + x_min

    readsize(1) = my_z_dim
    readsize(2) = my_y_dim
    readsize(3) = my_x_dim

!write (*,'i2 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    call adios_gclose (group_handle, adios_err)

    call adios_fclose (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    call MPI_Finalize (adios_err)
    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 i5 f') filename, 5, size, nproc_x, nproc_y, nproc_z, end_time - start_time
    endif
end subroutine

! 6. an arbitrary area on an orthogonal plane (decomposition dimensions)
subroutine read_pattern_6
    implicit none
#include "mpif.h"

    integer :: adios_err
    integer*8 :: adios_handle
    integer*8 :: group_handle
    integer*8 :: read_bytes
    character(len=256) :: filename, nx_str, ny_str, nz_str
    integer :: group_count
    integer :: var_count
    integer :: attr_count
    integer :: time_start
    integer :: time_steps
    integer :: var_type
    integer :: vtimed
    integer :: var_rank
    integer*8, dimension(10) :: dims
    integer*8, dimension(10) :: start
    integer*8, dimension(10) :: readsize
    character (len=100), dimension(5000) :: groupname_list
    character (len=100), dimension(5000) :: varname_list
    character (len=100), dimension(5000) :: attrname_list

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
    filename = trim(filename)//'.bp'

    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, size, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_fopen (adios_handle, filename, MPI_COMM_WORLD, group_count, adios_err)

    call adios_gopen (adios_handle, group_handle, 'data', var_count, attr_count, adios_err)

    call adios_read_var (group_handle, '/nx', start, readsize, nx, read_bytes)
    call adios_read_var (group_handle, '/ny', start, readsize, ny, read_bytes)
    call adios_read_var (group_handle, '/nz', start, readsize, nz, read_bytes)

    call adios_inq_var (group_handle, '/D', var_type, var_rank, dims, vtimed, adios_err)
    !if (rank == 0) write (*,'a9 i3 a9 i3 a9 i3') 'dims(1)', dims (1), 'dims(2)', dims(2), 'dims(3)', dims(3)
    !if (rank == 0) write (*, 'a3 i3 a3 i3 a3 i3 a9 i3 a9 i3') 'nx', nx, 'ny', ny, 'nz', nz, 'nproc_x', nproc_x, 'nproc_y', nproc_y

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a sub plane in dim(1)
    my_x_dim = ny / nproc_x / 2
    my_y_dim = nx / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = nz / 4
    start(2) = ny / 4 + x_min
    start(3) = nx / 4 + y_min

    readsize(1) = 1
    readsize(2) = my_x_dim
    readsize(3) = my_y_dim

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 1, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a sub plane in dim(2)
    my_x_dim = nz / nproc_x / 2
    my_y_dim = nx / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = nz / 4 + x_min
    start(2) = ny / 4
    start(3) = nx / 4 + y_min

    readsize(1) = my_x_dim
    readsize(2) = 1
    readsize(3) = my_y_dim

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 2, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! do a sub plane in dim(3)
    my_x_dim = nz / nproc_x / 2
    my_y_dim = ny / nproc_y / 2
    x_min = MOD(rank, nproc_x) * my_x_dim
    y_min = (rank/nproc_x) * my_y_dim

    start(1) = nz / 4 + x_min
    start(2) = ny / 4 + y_min
    start(3) = nx / 4

    readsize(1) = my_x_dim
    readsize(2) = my_y_dim
    readsize(3) = 1

!write (*,'i2 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3 a9 i3') 3, rank, 'start(1)',start(1),'start(2)',start(2),'start(3)',start(3),'size(1)',readsize(1),'size(2)',readsize(2),'size(3)',readsize(3)

    deallocate (grav_x_c)
    allocate (grav_x_c (readsize (1), readsize(2), readsize(3)))

    call adios_read_var (group_handle, '/D', start, readsize, grav_x_c, read_bytes)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    call adios_gclose (group_handle, adios_err)

    call adios_fclose (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    call MPI_Finalize (adios_err)
    if (rank == 0) then
        write (*,'a30 i2 i6 i5 i5 f') filename, 6, size, nproc_x, nproc_y, end_time - start_time
    endif
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
