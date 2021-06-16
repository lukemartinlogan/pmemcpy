module write_2d_data

public :: write_netcdf4

    integer :: nc_err
    character(len=256) :: filename_base, filename, ny_str, nz_str

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

    integer*8 :: total_written_size

contains

subroutine write_netcdf4
    use netcdf
    implicit none
    real*8 :: start_time, end_time, t_time, sz, gps
#include "mpif.h"

    integer :: ncid, nc_err
    integer :: nx_dimid, nx_1_dimid,ny_dimid, ny_1_dimid,nz_dimid, nz_1_dimid
    integer :: nez_dimid, nnu_dimid, nnu_1_dimid, nnc_dimid
    integer :: ij_ray_dimid, ik_ray_dimid, array_dimensions_dimid

    integer :: myid_varid
    integer :: array_dimensions_varid, time_varid, cycle_varid
    integer :: rho_c_varid, t_c_varid, ye_c_varid, u_c_varid, v_c_varid
    integer :: w_c_varid, nse_c_varid, a_nuc_rep_c_varid, z_nuc_rep_c_varid
    integer :: be_nuc_rep_c_varid, uburn_c_varid, duesrc_varid, pMD_varid
    integer :: sMD_varid, dudt_nuc_varid, dudt_nu_varid
    integer :: grav_x_c_varid, grav_y_c_varid, grav_z_c_varid
    integer :: agr_e_varid, agr_c_varid

    integer :: psi0_c_varid, psi1_e_varid, dnurad_varid

    integer :: nz_hyperslabs_varid, my_hyperslab_group_varid
    integer :: nz_hyperslab_width_varid
    integer :: radial_index_bound_varid, theta_index_bound_varid
    integer :: phi_index_bound_varid
    integer :: x_ef_varid
    integer :: y_ef_varid
    integer :: z_ef_varid
    integer :: x_cf_varid, dx_cf_varid
    integer :: y_cf_varid, dy_cf_varid
    integer :: z_cf_varid, dz_cf_varid
    integer :: unukrad_varid, nnukrad_varid, nu_r_varid, nu_rt_varid
    integer :: nu_rho_varid, nu_rhot_varid, psi0dat_varid, psi1dat_varid
    integer :: unujrad_varid, nnujrad_varid
    integer :: e_rad_varid, elec_rad_varid, r_shock_varid, r_shock_mn_varid
    integer :: r_shock_mx_varid, tau_adv_varid, tau_heat_nu_varid
    integer :: tau_heat_nuc_varid, r_nse_varid
    integer :: unurad_varid, nnurad_varid, rsphere_mean_varid
    integer :: dsphere_mean_varid, tsphere_mean_varid, msphere_mean_varid
    integer :: esphere_mean_varid
    integer :: e_nu_c_bar_varid, f_nu_e_bar_varid
    integer :: xn_c_varid
    integer :: r_gain_varid
    integer :: unu_c_varid, dunu_c_varid, unue_e_varid, dunue_e_varid

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    start_time = MPI_Wtime ()

    nc_err = nf90_create_par (filename, NF90_NETCDF4, MPI_COMM_WORLD, MPI_INFO_NULL, ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to create ', nf90_strerror (nc_err)

    nc_err = nf90_def_dim (ncid, "nx", nx, nx_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nx ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nx+1", nx+1, nx_1_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nx+1 ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "ny", ny, ny_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim ny ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "ny+1", ny+1, ny_1_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim ny+1 ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nz", nz, nz_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nz ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nz+1", nz+1, nz_1_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nz+1 ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nez", nez, nez_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nez ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nnu", nnu, nnu_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nnu ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nnu+1", nnu+1, nnu_1_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nnu+1 ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "nnc", nnc, nnc_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim nnc ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "ij_ray_dimid", my_j_ray_dim, ij_ray_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim ij_ray ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "ik_ray_dimid", my_k_ray_dim, ik_ray_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim ik_ray ', nf90_strerror (nc_err)
    nc_err = nf90_def_dim (ncid, "array_dimensions_dimid", 3, array_dimensions_dimid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def dim array_dimensions_dimid ', nf90_strerror (nc_err)

    nc_err = nf90_def_var (ncid, "myid", NF90_INT, varid=myid_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var myid ', nf90_strerror (nc_err)

    nc_err = nf90_def_var (ncid, "array_dimensions", NF90_INT, (/array_dimensions_dimid/), array_dimensions_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var array_dimensions ', nf90_strerror (nc_err)

    nc_err = nf90_def_var (ncid, "time", NF90_DOUBLE, varid=time_varid)
    nc_err = nf90_def_var (ncid, "cycle", NF90_INT, varid=cycle_varid)
    nc_err = nf90_def_var (ncid, "rho_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), rho_c_varid)
    nc_err = nf90_def_var (ncid, "t_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), t_c_varid)
    nc_err = nf90_def_var (ncid, "ye_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), ye_c_varid)
    nc_err = nf90_def_var (ncid, "u_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), u_c_varid)
    nc_err = nf90_def_var (ncid, "v_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), v_c_varid)
    nc_err = nf90_def_var (ncid, "w_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), w_c_varid)
    nc_err = nf90_def_var (ncid, "nse_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), nse_c_varid)
    nc_err = nf90_def_var (ncid, "a_nuc_rep_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), a_nuc_rep_c_varid)
    nc_err = nf90_def_var (ncid, "z_nuc_rep_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), z_nuc_rep_c_varid)
    nc_err = nf90_def_var (ncid, "be_nuc_rep_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), be_nuc_rep_c_varid)
    nc_err = nf90_def_var (ncid, "uburn_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), uburn_c_varid)
    nc_err = nf90_def_var (ncid, "duesrc", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), duesrc_varid)
    nc_err = nf90_def_var (ncid, "pMD", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), pMD_varid)
    nc_err = nf90_def_var (ncid, "sMD", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), sMD_varid)
    nc_err = nf90_def_var (ncid, "dudt_nuc", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), dudt_nuc_varid)
    nc_err = nf90_def_var (ncid, "dudt_nu", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), dudt_nu_varid)
    nc_err = nf90_def_var (ncid, "grav_x_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), grav_x_c_varid)
    nc_err = nf90_def_var (ncid, "grav_y_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), grav_y_c_varid)
    nc_err = nf90_def_var (ncid, "grav_z_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), grav_z_c_varid)
    nc_err = nf90_def_var (ncid, "agr_e", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), agr_e_varid)
    nc_err = nf90_def_var (ncid, "agr_c", NF90_DOUBLE, (/ nx_dimid, ny_dimid, nz_dimid /), agr_c_varid)

    nc_err = nf90_def_var (ncid, "psi0_c", NF90_DOUBLE, (/ nx_dimid, nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), psi0_c_varid)
    nc_err = nf90_def_var (ncid, "psi1_e", NF90_DOUBLE, (/ nx_dimid, nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), psi1_e_varid)
    nc_err = nf90_def_var (ncid, "dnurad", NF90_DOUBLE, (/ nx_dimid, nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), dnurad_varid)

!call adios_write (adios_handle, "j_ray_min", j_ray_min, nc_err)
!call adios_write (adios_handle, "j_ray_min-1", j_ray_min-1, nc_err)
!call adios_write (adios_handle, "k_ray_min", k_ray_min, nc_err)
!call adios_write (adios_handle, "k_ray_min-1", k_ray_min-1, nc_err)

    nc_err = nf90_def_var (ncid, "nz_hyperslabs", NF90_INT, varid=nz_hyperslabs_varid)
    nc_err = nf90_def_var (ncid, "my_hyperslab_group", NF90_INT, varid=my_hyperslab_group_varid)
    nc_err = nf90_def_var (ncid, "nz_hyperslab_width", NF90_INT, varid=nz_hyperslab_width_varid)

    nc_err = nf90_def_var (ncid, "radial_index_bound", NF90_INT, 2, radial_index_bound_varid)
    nc_err = nf90_def_var (ncid, "theta_index_bound", NF90_INT, 2, theta_index_bound_varid)
    nc_err = nf90_def_var (ncid, "phi_index_bound", NF90_INT, 2, phi_index_bound_varid)

    nc_err = nf90_def_var (ncid, "x_ef", NF90_DOUBLE, (/ nx_1_dimid /), x_ef_varid)

    nc_err = nf90_def_var (ncid, "y_ef", NF90_DOUBLE, (/ ny_1_dimid /), y_ef_varid)

    nc_err = nf90_def_var (ncid, "z_ef", NF90_DOUBLE, (/ nz_1_dimid /), z_ef_varid)

    nc_err = nf90_def_var (ncid, "x_cf", NF90_DOUBLE, (/ nx_dimid /), x_cf_varid)
    nc_err = nf90_def_var (ncid, "dx_cf", NF90_DOUBLE, (/ nx_dimid /), dx_cf_varid)

    nc_err = nf90_def_var (ncid, "y_cf", NF90_DOUBLE, (/ ny_dimid /), y_cf_varid)
    nc_err = nf90_def_var (ncid, "dy_cf", NF90_DOUBLE, (/ ny_dimid /), dy_cf_varid)

    nc_err = nf90_def_var (ncid, "z_cf", NF90_DOUBLE, (/ nz_dimid /), z_cf_varid)
    nc_err = nf90_def_var (ncid, "dz_cf", NF90_DOUBLE, (/ nz_dimid /), dz_cf_varid)

    nc_err = nf90_def_var (ncid, "unukrad", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), unukrad_varid)
    nc_err = nf90_def_var (ncid, "nu_r", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), nu_r_varid)
    nc_err = nf90_def_var (ncid, "nu_rt", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), nu_rt_varid)
    nc_err = nf90_def_var (ncid, "nu_rho", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), nu_rho_varid)
    nc_err = nf90_def_var (ncid, "nu_rhot", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), nu_rhot_varid)
    nc_err = nf90_def_var (ncid, "psi0dat", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), psi0dat_varid)
    nc_err = nf90_def_var (ncid, "psi1dat", NF90_DOUBLE, (/nez_dimid, nnu_dimid, ny_dimid, nz_dimid /), psi1dat_varid)

    nc_err = nf90_def_var (ncid, "unujrad", NF90_DOUBLE, (/ nx_dimid, nnu_dimid, ny_dimid, nz_dimid /), unujrad_varid)
    nc_err = nf90_def_var (ncid, "nnujrad", NF90_DOUBLE, (/ nx_dimid, nnu_dimid, ny_dimid, nz_dimid /), nnujrad_varid)

    nc_err = nf90_def_var (ncid, "e_rad", NF90_double, (/ ny_dimid, nz_dimid /), e_rad_varid)
    nc_err = nf90_def_var (ncid, "elec_rad", NF90_double, (/ ny_dimid, nz_dimid /), elec_rad_varid)
    nc_err = nf90_def_var (ncid, "r_shock", NF90_double, (/ ny_dimid, nz_dimid /), r_shock_varid)
    nc_err = nf90_def_var (ncid, "r_shock_mn", NF90_double, (/ ny_dimid, nz_dimid /), r_shock_mn_varid)
    nc_err = nf90_def_var (ncid, "r_shock_mx", NF90_double, (/ ny_dimid, nz_dimid /), r_shock_mx_varid)
    nc_err = nf90_def_var (ncid, "tau_adv", NF90_double, (/ ny_dimid, nz_dimid /), tau_adv_varid)
    nc_err = nf90_def_var (ncid, "tau_heat_nu", NF90_double, (/ ny_dimid, nz_dimid /), tau_heat_nu_varid)
    nc_err = nf90_def_var (ncid, "tau_heat_nuc", NF90_double, (/ ny_dimid, nz_dimid /), tau_heat_nuc_varid)
    nc_err = nf90_def_var (ncid, "r_nse", NF90_double, (/ ny_dimid, nz_dimid /), r_nse_varid)

    nc_err = nf90_def_var (ncid, "unurad", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), unurad_varid)
    nc_err = nf90_def_var (ncid, "nnurad", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), nnurad_varid)
    nc_err = nf90_def_var (ncid, "rsphere_mean", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), rsphere_mean_varid)
    nc_err = nf90_def_var (ncid, "dsphere_mean", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), dsphere_mean_varid)
    nc_err = nf90_def_var (ncid, "tsphere_mean", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), tsphere_mean_varid)
    nc_err = nf90_def_var (ncid, "msphere_mean", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), msphere_mean_varid)
    nc_err = nf90_def_var (ncid, "esphere_mean", NF90_DOUBLE, (/ nnu_dimid, ny_dimid, nz_dimid /), esphere_mean_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var esphere_mean ', nf90_strerror (nc_err)
    nc_err = nf90_def_var (ncid, "e_nu_c_bar", NF90_DOUBLE, (/ nx_dimid /), e_nu_c_bar_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var e_nu_c_bar ', nf90_strerror (nc_err)
    nc_err = nf90_def_var (ncid, "f_nu_e_bar", NF90_DOUBLE, (/ nx_dimid /), f_nu_e_bar_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var f_nu_e_bar ', nf90_strerror (nc_err)

    nc_err = nf90_def_var (ncid, "xn_c", NF90_DOUBLE, (/ nx_dimid, nnc_dimid, ny_dimid, nz_dimid /), xn_c_varid)

    nc_err = nf90_def_var (ncid, "r_gain", NF90_DOUBLE, (/ nnu_1_dimid, ny_dimid, nz_dimid /), r_gain_varid)

    nc_err = nf90_def_var (ncid, "unu_c", NF90_DOUBLE, (/ nx_dimid, nez_dimid, ny_dimid, nz_dimid /), unu_c_varid)
    nc_err = nf90_def_var (ncid, "dunu_c", NF90_DOUBLE, (/ nx_dimid, nez_dimid, ny_dimid, nz_dimid /), dunu_c_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var dunu_c ', nf90_strerror (nc_err)
    nc_err = nf90_def_var (ncid, "unue_e", NF90_DOUBLE, (/ nx_dimid, nez_dimid, ny_dimid, nz_dimid /), unue_e_varid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to def var unue_e ', nf90_strerror (nc_err)

    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to enddef ', nf90_strerror (nc_err)

    nc_err = nf90_def_var (ncid, "dunue_e", NF90_DOUBLE, (/ nx_dimid, nez_dimid, ny_dimid, nz_dimid /), dunue_e_varid)

    nc_err = nf90_enddef (ncid)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (rank == 0) then
    nc_err = nf90_var_par_access (ncid, myid_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, array_dimensions_varid, NF90_INDEPENDENT);

    nc_err = nf90_put_var (ncid, myid_varid, rank)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to put var myid ', nf90_strerror (nc_err)
    nc_err = nf90_put_var (ncid, array_dimensions_varid, array_dimensions, (/ 1 /), (/ 3 /))
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to put var array_dimensions ', nf90_strerror (nc_err)

    nc_err = nf90_var_par_access (ncid, time_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, cycle_varid, NF90_INDEPENDENT);

    nc_err = nf90_put_var (ncid, time_varid, time)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to put var time ', nf90_strerror (nc_err)
    nc_err = nf90_put_var (ncid, cycle_varid, ncycle)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to put var cycle ', nf90_strerror (nc_err)
    endif

    nc_err = nf90_put_var (ncid, rho_c_varid, rho_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, t_c_varid, t_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, ye_c_varid, ye_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, u_c_varid, u_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, v_c_varid, v_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, w_c_varid, w_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, nse_c_varid, nse_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, a_nuc_rep_c_varid, a_nuc_rep_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, z_nuc_rep_c_varid, z_nuc_rep_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, be_nuc_rep_c_varid, be_nuc_rep_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, uburn_c_varid, uburn_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, duesrc_varid, duesrc, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, pMD_varid, pMD, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, sMD_varid, sMD, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, dudt_nuc_varid, dudt_nuc, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, dudt_nu_varid, dudt_nu, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, grav_x_c_varid, grav_x_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, grav_y_c_varid, grav_y_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, grav_z_c_varid, grav_z_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, agr_e_varid, agr_e, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 
    nc_err = nf90_put_var (ncid, agr_c_varid, agr_c, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, my_j_ray_dim, my_k_ray_dim /)) 

    nc_err = nf90_put_var (ncid, psi0_c_varid, psi0_c, start = (/ 0, 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, psi1_e_varid, psi1_e, start = (/ 0, 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, dnurad_varid, dnurad, start = (/ 0, 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, nnu, my_j_ray_dim, my_k_ray_dim /))

!***************************************************
    if (rank == 0) then
    nc_err = nf90_var_par_access (ncid, nz_hyperslabs_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, my_hyperslab_group_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, nz_hyperslab_width, NF90_INDEPENDENT);

    nc_err = nf90_put_var (ncid, nz_hyperslabs_varid, nz_hyperslabs)
    nc_err = nf90_put_var (ncid, my_hyperslab_group_varid, my_hyperslab_group)
    nc_err = nf90_put_var (ncid, nz_hyperslab_width, nz_hyperslab_width)

    nc_err = nf90_var_par_access (ncid, radial_index_bound_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, theta_index_bound_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, phi_index_bound_varid, NF90_INDEPENDENT);

    nc_err = nf90_put_var (ncid, radial_index_bound_varid, radial_index_bound, start = (/ 0 /), count = (/ 2 /))
    nc_err = nf90_put_var (ncid, theta_index_bound_varid, theta_index_bound, start = (/ 0 /), count = (/ 2 /))
    nc_err = nf90_put_var (ncid, phi_index_bound_varid, phi_index_bound, start = (/ 0 /), count = (/ 2 /))
    endif
!***************************************************

    nc_err = nf90_put_var (ncid, x_ef_varid, x_ef, start = (/ 0 /), count = (/ nx+1 /))

    nc_err = nf90_put_var (ncid, y_ef_varid, y_ef, start = (/ 0 /), count = (/ ny+1 /))

    nc_err = nf90_put_var (ncid, z_ef_varid, z_ef,  start = (/ 0 /), count = (/ nz+1 /))

    nc_err = nf90_put_var (ncid, x_cf_varid, x_cf, start = (/ 0 /), count = (/ nx /))
    nc_err = nf90_put_var (ncid, dx_cf_varid, dx_cf, start = (/ 0 /), count = (/ nx /))

    nc_err = nf90_put_var (ncid, y_cf_varid, y_cf, start = (/ 0 /), count = (/ ny /))
    nc_err = nf90_put_var (ncid, dy_cf_varid, dy_cf, start = (/ 0 /), count = (/ ny /))

    nc_err = nf90_put_var (ncid, z_cf_varid, z_cf, start = (/ 0 /), count = (/ nz /))
    nc_err = nf90_put_var (ncid, dz_cf_varid, dz_cf, start = (/ 0 /), count = (/ nz /))

    nc_err = nf90_put_var (ncid, unukrad_varid, unukrad, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nu_r_varid, nu_r, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nu_rt_varid, nu_rt, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nu_rho_varid, nu_rho, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nu_rhot_varid, nu_rhot, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, psi0dat_varid, psi0dat, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, psi1dat_varid, psi1dat, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nez, nnu, my_j_ray_dim, my_k_ray_dim /))

    nc_err = nf90_put_var (ncid, unujrad_varid, unujrad, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nnujrad_varid, nnujrad, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nnu, my_j_ray_dim, my_k_ray_dim /))

    nc_err = nf90_put_var (ncid, e_rad_varid, e_rad, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, elec_rad_varid, elec_rad, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, r_shock_varid, r_shock, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, r_shock_mn_varid, r_shock_mn, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, r_shock_mx_varid, r_shock_mx, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, tau_adv_varid, tau_adv, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, tau_heat_nu_varid, tau_heat_nu, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, tau_heat_nuc_varid, tau_heat_nuc, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, r_nse_varid, r_nse, start = (/ j_ray_min-1, k_ray_min-1 /), count = (/ my_j_ray_dim, my_k_ray_dim /))

    nc_err = nf90_put_var (ncid, unurad_varid, unurad, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, nnurad_varid, nnurad, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, rsphere_mean_varid, rsphere_mean, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, dsphere_mean_varid, dsphere_mean, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, tsphere_mean_varid, tsphere_mean, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, msphere_mean_varid, msphere_mean, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, esphere_mean_varid, esphere_mean, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu, my_j_ray_dim, my_k_ray_dim /))

    if (rank == 0) then
    nc_err = nf90_var_par_access (ncid, e_nu_c_bar_varid, NF90_INDEPENDENT);
    nc_err = nf90_var_par_access (ncid, f_nu_e_bar_varid, NF90_INDEPENDENT);

    nc_err = nf90_put_var (ncid, e_nu_c_bar_varid, e_nu_c_bar, start = (/ 0 /), count = (/ nx /))
    nc_err = nf90_put_var (ncid, f_nu_e_bar_varid, f_nu_e_bar, start = (/ 0 /), count = (/ nx /))
    endif

    nc_err = nf90_put_var (ncid, xn_c_varid, xn_c, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nnc, my_j_ray_dim, my_k_ray_dim /))

    nc_err = nf90_put_var (ncid, r_gain_varid, r_gain, start = (/ 0, j_ray_min-1, k_ray_min-1 /), count = (/ nnu+1, my_j_ray_dim,my_k_ray_dim /))

    nc_err = nf90_put_var (ncid, unu_c_varid, unu_c, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, dunu_c_varid, dunu_c, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, unue_e_varid, unue_e, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, my_j_ray_dim, my_k_ray_dim /))
    nc_err = nf90_put_var (ncid, dunue_e_varid, dunue_e, start = (/ 0, 0, j_ray_min-1, k_ray_min-1 /), count = (/ nx, nez, my_j_ray_dim, my_k_ray_dim /))
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = nf90_close (ncid)
    if (nc_err /= nf90_noerr) write (*,*) rank, ' failure to nf90_close ', nf90_strerror (nc_err)

    call MPI_Barrier (MPI_COMM_WORLD, nc_err)
    end_time = MPI_Wtime ()

    t_time = end_time - start_time;
    sz = total_written_size / (1024.0 * 1024.0 * 1024.0)
    gps = sz / t_time;

    if (rank == 0) then
        write (*,'a10 i5 f x f x f') filename, nproc, sz, t_time, gps
    endif

    return
end subroutine

end module

program write_2d
    use write_2d_data
    implicit none

    integer :: nproc_y, nproc_z

#include "mpif.h"
    call MPI_Init (nc_err)
    call MPI_Comm_rank (MPI_COMM_WORLD, rank, nc_err)
    call MPI_Comm_size (MPI_COMM_WORLD, nproc, nc_err)

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

    total_written_size = 0
    ! start with 25 integers
    total_written_size = 4 * 25

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

    total_written_size = total_written_size + (2 * 8 * nx)

    x_cf = rank
    dx_cf = rank

    allocate (y_cf (1:my_j_ray_dim))
    allocate (dy_cf (1:my_j_ray_dim))

    total_written_size = total_written_size + (2 * 8 * my_j_ray_dim)

    y_cf = rank
    dy_cf = rank

    allocate (z_cf (1:my_k_ray_dim))
    allocate (dz_cf (1:my_k_ray_dim))

    total_written_size = total_written_size + (2 * 8 * my_k_ray_dim)

    z_cf = rank
    dz_cf = rank

    allocate (x_ef (1:nx+1))
    allocate (y_ef (1:my_j_ray_dim+1))
    allocate (z_ef (1:my_k_ray_dim+1))

    total_written_size = total_written_size + (8 * (nx + 1))
    total_written_size = total_written_size + (8 * (my_j_ray_dim + 1))
    total_written_size = total_written_size + (8 * (my_k_ray_dim + 1))

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

    total_written_size = total_written_size + (21 * 8 * nx)

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

    total_written_size = total_written_size + (3 * 8 * nx * nez * nnu * my_j_ray_dim * my_k_ray_dim)

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

    total_written_size = total_written_size + (8 * 8 * nez * nnu * my_j_ray_dim * my_k_ray_dim)

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

    total_written_size = total_written_size + (2 * 8 * nx * nnu * my_j_ray_dim * my_k_ray_dim)

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

    total_written_size = total_written_size + (9 * 8 * 1)

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

    total_written_size = total_written_size + (7 * 8 * nnu)

    unurad = rank
    nnurad = rank
    rsphere_mean = rank
    dsphere_mean = rank
    tsphere_mean = rank
    msphere_mean = rank
    esphere_mean = rank

    allocate (e_nu_c_bar (1:nx))
    allocate (f_nu_e_bar (1:nx))

    total_written_size = total_written_size + (2 * 8 * nx)

    e_nu_c_bar = rank
    f_nu_e_bar = rank

    allocate (xn_c (1:nx,1:nnc,1:my_j_ray_dim,1:my_k_ray_dim))

    total_written_size = total_written_size + (1 * 8 * nx * nnc * my_j_ray_dim * my_k_ray_dim)

    xn_c = rank

    allocate (r_gain (1:nnu+1,1:my_j_ray_dim,1:my_k_ray_dim))

    total_written_size = total_written_size + (1 * 8 * (nnu+1) * my_j_ray_dim * my_k_ray_dim)

    r_gain = rank

    allocate (unu_c (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (dunu_c (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (unue_e (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))
    allocate (dunue_e (1:nx,1:nez,1:my_j_ray_dim,1:my_k_ray_dim))

    total_written_size = total_written_size + (4 * 8 * nx * nez * my_j_ray_dim * my_k_ray_dim)

    unu_c = rank
    dunu_c = rank
    unue_e = rank
    dunue_e = rank

    total_written_size = total_written_size * nproc

    filename = trim(filename_base)//".nc4"
    call write_netcdf4 ()

    call MPI_Finalize (nc_err)
end program
