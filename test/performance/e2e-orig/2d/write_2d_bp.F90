module write_2d_data

public :: write_adios

    character(len=256) :: filename_base, filename, ny_str, nz_str
    integer*8 :: adios_handle, adios_groupsize, adios_totalsize, adios_err

    integer :: nx, ny, nz
    integer :: nez, nnu, nnc
    integer :: my_j_ray_dim, my_k_ray_dim

    integer :: j_ray_min, k_ray_min

    real*8, dimension (:), allocatable :: x_cf, y_cf, z_cf
    real*8, dimension (:), allocatable :: dx_cf, dy_cf, dz_cf
    real*8, dimension (:), allocatable :: x_ef, y_ef, z_ef

    integer :: ncycle, time

    integer :: myid

    real*8, dimension (:,:,:), allocatable :: t_c, u_c, v_c, w_c, rho_c, ye_c, nse_c

    integer :: array_dimensions (3)

    integer :: nz_hyperslabs, my_hyperslab_group, nz_hyperslab_width
    integer :: radial_index_bound (2), theta_index_bound (2), phi_index_bound (2)

    real*8, dimension (:,:,:), allocatable :: a_nuc_rep_c, z_nuc_rep_c, be_nuc_rep_c
    real*8, dimension (:,:,:), allocatable :: uburn_c, duesrc, pMD, sMD
    real*8, dimension (:,:,:), allocatable :: dudt_nuc, dudt_nu
    real*8, dimension (:,:,:), allocatable :: grav_x_c, grav_y_c, grav_z_c
    real*8, dimension (:,:,:), allocatable :: agr_e, agr_c

    real*8, dimension (:,:,:,:,:), allocatable :: psi0_c, psi1_e
    real*8, dimension (:,:,:,:,:), allocatable :: dnurad

    real*8, dimension (:,:,:,:), allocatable :: unukrad, nnukrad
    real*8, dimension (:,:,:,:), allocatable :: nu_r, nu_rt, nu_rho, nu_rhot
    real*8, dimension (:,:,:,:), allocatable :: psi0dat, psi1dat

    real*8, dimension (:,:,:,:), allocatable :: unujrad, nnujrad

    real*8, dimension (:,:), allocatable :: e_rad, elec_rad
    real*8, dimension (:,:), allocatable :: r_shock, r_shock_mn, r_shock_mx, tau_adv
    real*8, dimension (:,:), allocatable :: tau_heat_nu, tau_heat_nuc, r_nse


    real*8, dimension (:,:,:), allocatable :: unurad, nnurad
    real*8, dimension (:,:,:), allocatable :: rsphere_mean, dsphere_mean, tsphere_mean, msphere_mean, esphere_mean

    real*8, dimension (:), allocatable :: e_nu_c_bar, f_nu_e_bar

    real*8, dimension (:,:,:,:), allocatable :: xn_c

    real*8, dimension (:,:,:), allocatable :: r_gain

    real*8, dimension (:,:,:,:), allocatable :: unu_c, dunu_c, unue_e, dunue_e

    integer :: rank, nproc

contains

subroutine write_adios
    implicit none
#include "mpif.h"
    real*8 :: start_time, end_time, t_time, sz, gps

    call adios_init ('2d.xml', adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    start_time = MPI_Wtime ()

    call adios_open (adios_handle, 'restart.model', filename, "w", MPI_COMM_WORLD, adios_err)
#include "gwrite_restart.model.fh"
    call adios_close (adios_handle, adios_err)

    call MPI_Barrier (MPI_COMM_WORLD, adios_err)
    end_time = MPI_Wtime ()

    t_time = end_time - start_time;
    sz = adios_groupsize / (1024.0 * 1024.0 * 1024.0) * nproc;
    gps = sz / t_time;

    if (rank == 0) then
        write (*,'a10 i5 f x f x f') filename, nproc, sz, t_time, gps
    endif

    call adios_finalize (rank, adios_err)

    return
end subroutine

end module

program write_2d
    use write_2d_data
    implicit none

    integer :: nproc_y, nproc_z

#include "mpif.h"
    call MPI_Init (adios_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, adios_err)
    call MPI_Comm_size (MPI_COMM_WORLD, nproc, adios_err)

    call getarg (1, filename_base)
    call getarg (2, ny_str)
    call getarg (3, nz_str)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!      nx = size(x_cf)
!      ny = size(y_cf)
!      nz = size(z_cf)
!      nez = size(psi0_c, dim=2)
!      nnu = size(psi0_c, dim=3)
!      nnc = size(xn_c, dim=2)

!      CALL MPI_COMM_SIZE(MPI_COMM_ROW, nproc_y, error)
!      CALL MPI_COMM_SIZE(MPI_COMM_COL, nproc_z, error)
!      my_j_ray_dim = ny/nproc_y
!      my_k_ray_dim = nz/nproc_z

!      j_ray_min = MOD(myid, nproc_y) * my_j_ray_dim + 1
!      k_ray_min = (myid/nproc_y) * my_k_ray_dim + 1
!      j_ray_max = MOD(myid, nproc_y) * my_j_ray_dim + my_j_ray_dim
!      k_ray_max = (myid/nproc_y) * my_k_ray_dim + my_k_ray_dim
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nx = 300;
    read (ny_str, '(i6)') ny
    read (nz_str, '(i6)') nz
    nez = 20
    nnu = 4
    nnc = 17
    nproc_y = ny
    nproc_z = nz

    myid = rank

    my_j_ray_dim = 4
    my_k_ray_dim = 4
    ny = nproc_y * my_j_ray_dim
    nz = nproc_z * my_k_ray_dim

    j_ray_min = MOD(myid, nproc_y) * my_j_ray_dim + 1
    k_ray_min = (myid/nproc_y) * my_k_ray_dim + 1

!write (*, 'i4 a9 i3 a9 i3 a9 i3 a8 i3 a8 i3 a8 i3') myid, 'start(1)', 0, 'start(2)', j_ray_min, 'start(3)', k_ray_min, 'size(1)', 300, 'size(2)', my_j_ray_dim, 'size(3)', my_k_ray_dim

    allocate (x_cf (1:nx))
    allocate (dx_cf (1:nx))

    x_cf = rank
    dx_cf = rank

    allocate (y_cf (1:my_j_ray_dim))
    allocate (dy_cf (1:my_j_ray_dim))

    y_cf = rank
    dy_cf = rank

    allocate (z_cf (1:my_k_ray_dim))
    allocate (dz_cf (1:my_k_ray_dim))

    z_cf = rank
    dz_cf = rank

    allocate (x_ef (1:nx+1))
    allocate (y_ef (1:my_j_ray_dim+1))
    allocate (z_ef (1:my_k_ray_dim+1))

    x_ef = rank
    y_ef = rank
    z_ef = rank

    array_dimensions (1) = nx
    array_dimensions (2) = ny
    array_dimensions (3) = nz

    nz_hyperslabs = 1
    my_hyperslab_group = rank ! calc from procid
    nz_hyperslab_width = my_k_ray_dim
    radial_index_bound (1) = 1
    radial_index_bound (2) = 256
    theta_index_bound (1) = 1
    theta_index_bound (2) = ny
    phi_index_bound (1) = 1
    phi_index_bound (2) = nz
    ncycle = 50
    time = 0.00679921

    allocate (t_c (1,1,1:nx))
    allocate (u_c (1,1,1:nx))
    allocate (v_c (1,1,1:nx))
    allocate (w_c (1,1,1:nx))
    allocate (rho_c (1,1,1:nx))
    allocate (ye_c (1,1,1:nx))
    allocate (nse_c (1,1,1:nx))
    allocate (a_nuc_rep_c (1,1,1:nx))
    allocate (z_nuc_rep_c (1,1,1:nx))
    allocate (be_nuc_rep_c (1,1,1:nx))
    allocate (uburn_c (1,1,1:nx))
    allocate (duesrc (1,1,1:nx))
    allocate (pMD (1,1,1:nx))
    allocate (sMD (1,1,1:nx))
    allocate (dudt_nuc (1,1,1:nx))
    allocate (dudt_nu (1,1,1:nx))
    allocate (grav_x_c (1,1,1:nx))
    allocate (grav_y_c (1,1,1:nx))
    allocate (grav_z_c (1,1,1:nx))
    allocate (agr_e (1,1,1:nx))
    allocate (agr_c (1,1,1:nx))

    t_c = rank
    u_c = rank
    v_c = rank
    w_c = rank
    rho_c = rank
    ye_c = rank
    nse_c = rank
    a_nuc_rep_c = rank
    z_nuc_rep_c = rank
    be_nuc_rep_c = rank
    uburn_c = rank
    duesrc = rank
    pMD = rank
    sMD = rank
    dudt_nuc = rank
    dudt_nu = rank
    grav_x_c = rank
    grav_y_c = rank
    grav_z_c = rank
    agr_e = rank
    agr_c = rank

    allocate (psi0_c (1:nx,1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (psi1_e (1:nx,1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (dnurad (1:nx,1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))

    psi0_c = rank
    psi1_e = rank
    dnurad = rank

    allocate (unukrad (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nnukrad (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nu_r (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nu_rt (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nu_rho (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nu_rhot (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (psi0dat (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (psi1dat (1:nez,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))

    unukrad = rank
    nnukrad = rank
    nu_r = rank
    nu_rt = rank
    nu_rho = rank
    nu_rhot = rank
    psi0dat = rank
    psi1dat = rank

    allocate (unujrad (1:nx,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (nnujrad (1:nx,1:nnu,1:my_j_ray_dim,1:my_k_ray_dim))

    unujrad = rank
    nnujrad = rank

    allocate (e_rad (1,1))
    allocate (elec_rad (1,1))
    allocate (r_shock (1,1))
    allocate (r_shock_mn (1,1))
    allocate (r_shock_mx (1,1))
    allocate (tau_adv (1,1))
    allocate (tau_heat_nu (1,1))
    allocate (tau_heat_nuc (1,1))
    allocate (r_nse (1,1))

    e_rad = rank
    elec_rad = rank
    r_shock = rank
    r_shock_mn = rank
    r_shock_mx = rank
    tau_adv = rank
    tau_heat_nu = rank
    tau_heat_nuc = rank
    r_nse = rank

    allocate (unurad (1:nnu,1,1))
    allocate (nnurad (1:nnu,1,1))
    allocate (rsphere_mean (1:nnu,1,1))
    allocate (dsphere_mean (1:nnu,1,1))
    allocate (tsphere_mean (1:nnu,1,1))
    allocate (msphere_mean (1:nnu,1,1))
    allocate (esphere_mean (1:nnu,1,1))

    unurad = rank
    nnurad = rank
    rsphere_mean = rank
    dsphere_mean = rank
    tsphere_mean = rank
    msphere_mean = rank
    esphere_mean = rank

    allocate (e_nu_c_bar (1:nx))
    allocate (f_nu_e_bar (1:nx))

    e_nu_c_bar = rank
    f_nu_e_bar = rank

    allocate (xn_c (1:nx,1:nnc,1:my_j_ray_dim,1:my_k_ray_dim))

    xn_c = rank

    allocate (r_gain (1:nnu+1,1:my_j_ray_dim,1:my_k_ray_dim))

    r_gain = rank

    allocate (unu_c (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (dunu_c (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (unue_e (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (dunue_e (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))

    unu_c = rank
    dunu_c = rank
    unue_e = rank
    dunue_e = rank

    filename = trim(filename_base)//".bp"
    !write (*,*) rank, trim(filename), ' start'
    call write_adios ()
    !write (*,*) rank, trim(filename), ' done'

    call MPI_Finalize (adios_err)
end program

