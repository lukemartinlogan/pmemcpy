#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnetcdf.h>
#include <string.h>

int main (int argc, char ** argv)
{
    int rank;
    int size;
    MPI_Info info;
    char filename [256];

    int i;

    // vars to write
    int nx, ny, nz, nproc_y, nproc_z;
    int nez, nnu, nnc;
    int my_j_ray_dim, my_k_ray_dim;

    int j_ray_min, k_ray_min;

    double * x_cf, * y_cf, * z_cf;
    double * dx_cf, * dy_cf, * dz_cf;
    double * x_ef, * y_ef, * z_ef;

    int ncycle;
    double time;

    int myid;

    double * t_c, * u_c, * v_c, * w_c, * rho_c, * ye_c, * nse_c;

    int array_dimensions [3];

    int nz_hyperslabs, my_hyperslab_group, nz_hyperslab_width;
    int radial_index_bound [2], theta_index_bound [2], phi_index_bound [2];

    double * a_nuc_rep_c, * z_nuc_rep_c, * be_nuc_rep_c;
    double * uburn_c, * duesrc, * pMD, * sMD;
    double * dudt_nuc, * dudt_nu;
    double * grav_x_c, * grav_y_c, * grav_z_c;
    double * agr_e, * agr_c;

    double * psi0_c, * psi1_e;
    double * dnurad;

    double * unukrad, * nnukrad;
    double * nu_r, * nu_rt, * nu_rho, * nu_rhot;
    double * psi0dat, * psi1dat;

    double * unujrad, * nnujrad;

    double * e_rad, * elec_rad;
    double * r_shock, * r_shock_mn, * r_shock_mx, * tau_adv;
    double * tau_heat_nu, * tau_heat_nuc, * r_nse;

    double * unurad, * nnurad;
    double * rsphere_mean, * dsphere_mean, * tsphere_mean, * msphere_mean, * esphere_mean;

    double * e_nu_c_bar, * f_nu_e_bar;

    double * xn_c;

    double * r_gain;

    double * unu_c, * dunu_c, * unue_e, * dunue_e;

    int64_t total_written_size = 0;
    double start_time, end_time, t_time, sz, gps;

//////////////////////////////////////
    int ncid, nc_err;
    int nx_dimid, nx_1_dimid,ny_dimid, ny_1_dimid,nz_dimid, nz_1_dimid;
    int nez_dimid, nnu_dimid, nnu_1_dimid, nnc_dimid;
    int ij_ray_dimid, ik_ray_dimid;

    int myid_varid;
    int array_dimensions_varid, time_varid, cycle_varid;
    int rho_c_varid, t_c_varid, ye_c_varid, u_c_varid, v_c_varid;
    int w_c_varid, nse_c_varid, a_nuc_rep_c_varid, z_nuc_rep_c_varid;
    int be_nuc_rep_c_varid, uburn_c_varid, duesrc_varid, pMD_varid;
    int sMD_varid, dudt_nuc_varid, dudt_nu_varid;
    int grav_x_c_varid, grav_y_c_varid, grav_z_c_varid;
    int agr_e_varid, agr_c_varid;

    int psi0_c_varid, psi1_e_varid, dnurad_varid;

    int nz_hyperslabs_varid, my_hyperslab_group_varid;
    int nz_hyperslab_width_varid;
    int radial_index_bound_varid, theta_index_bound_varid;
    int phi_index_bound_varid;
    int x_ef_varid;
    int y_ef_varid;
    int z_ef_varid;
    int x_cf_varid, dx_cf_varid;
    int y_cf_varid, dy_cf_varid;
    int z_cf_varid, dz_cf_varid;
    int unukrad_varid, nnukrad_varid, nu_r_varid, nu_rt_varid;
    int nu_rho_varid, nu_rhot_varid, psi0dat_varid, psi1dat_varid;
    int unujrad_varid, nnujrad_varid;
    int e_rad_varid, elec_rad_varid, r_shock_varid, r_shock_mn_varid;
    int r_shock_mx_varid, tau_adv_varid, tau_heat_nu_varid;
    int tau_heat_nuc_varid, r_nse_varid;
    int unurad_varid, nnurad_varid, rsphere_mean_varid;
    int dsphere_mean_varid, tsphere_mean_varid, msphere_mean_varid;
    int esphere_mean_varid;
    int e_nu_c_bar_varid, f_nu_e_bar_varid;
    int xn_c_varid;
    int r_gain_varid;
    int unu_c_varid, dunu_c_varid, unue_e_varid, dunue_e_varid;

//////////////////////////////////////
    MPI_Init (&argc, &argv);

    MPI_Info_create(&info);
    MPI_Info_set(info, "cb_align", "2");
    MPI_Info_set(info, "romio_ds_write", "disable");

    sprintf (filename, "%s.nc5", argv [1]);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!      nx = size(x_cf)
//!      ny = size(y_cf)
//!      nz = size(z_cf)
//!      nez = size(psi0_c, dim=2)
//!      nnu = size(psi0_c, dim=3)
//!      nnc = size(xn_c, dim=2)
//
//!      CALL MPI_COMM_SIZE(MPI_COMM_ROW, nproc_y, error)
//!      CALL MPI_COMM_SIZE(MPI_COMM_COL, nproc_z, error)
//!      my_j_ray_dim = ny/nproc_y
//!      my_k_ray_dim = nz/nproc_z
//
//!      j_ray_min = MOD(myid, nproc_y) * my_j_ray_dim + 1
//!      k_ray_min = (myid/nproc_y) * my_k_ray_dim + 1
//!      j_ray_max = MOD(myid, nproc_y) * my_j_ray_dim + my_j_ray_dim
//!      k_ray_max = (myid/nproc_y) * my_k_ray_dim + my_k_ray_dim
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    myid = rank;
    nx = 300;
    ny = atoi (argv [2]);
    nz = atoi (argv [3]);
    nez = 20;
    nnu = 4;
    nnc = 17;
    nproc_y = ny;
    nproc_z = nz;

    my_j_ray_dim = 4;
    my_k_ray_dim = 4;
    ny = my_j_ray_dim * nproc_y;
    nz = my_k_ray_dim * nproc_z;

    j_ray_min = (myid % nproc_y) * my_j_ray_dim + 1;
    k_ray_min = (myid / nproc_y) * my_k_ray_dim + 1;

//printf ("%3d start(1) %3d start(2) %3d start(3) %3d size(1) %3d size(2) %3d size(3) %3d\n", rank, 0, j_ray_min, k_ray_min, nx, my_j_ray_dim, my_k_ray_dim);

    total_written_size = (25 * sizeof (int));

    x_cf = malloc (sizeof (double) * nx);
    dx_cf = malloc (sizeof (double) * nx);

    total_written_size += (2 * sizeof (double) * nx);

    for (i = 0; i < nx; i++)
    {
        x_cf [i] = rank;
        dx_cf [i] = rank;
    }

    y_cf = malloc (sizeof (double) * my_j_ray_dim);
    dy_cf = malloc (sizeof (double) * my_j_ray_dim);

    total_written_size += (2 * sizeof (double) * my_j_ray_dim);

    for (i = 0; i < my_j_ray_dim; i++)
    {
        y_cf [i] = rank;
        dy_cf [i] = rank;
    }

    z_cf = malloc (sizeof (double) * my_k_ray_dim);
    dz_cf = malloc (sizeof (double) * my_k_ray_dim);

    total_written_size += (2 * sizeof (double) * my_k_ray_dim);

    for (i = 0; i < my_k_ray_dim; i++)
    {
        z_cf [i] = rank;
        dz_cf [i] = rank;
    }

    x_ef = malloc (sizeof (double) * (nx + 1));
    y_ef = malloc (sizeof (double) * (my_j_ray_dim + 1));
    z_ef = malloc (sizeof (double) * (my_k_ray_dim + 1));

    total_written_size += (sizeof (double) * (nx + 1));
    total_written_size += (sizeof (double) * my_j_ray_dim);
    total_written_size += (sizeof (double) * my_k_ray_dim);

    for (i = 0; i < nx + 1; i++)
        x_ef [i] = rank;
    for (i = 0; i < my_j_ray_dim + 1; i++)
        y_ef [i] = rank;
    for (i = 0; i < my_k_ray_dim + 1; i++)
        z_ef [i] = rank;

    array_dimensions [0] = nx;
    array_dimensions [1] = ny;
    array_dimensions [2] = nz;

    nz_hyperslabs = 1;
    my_hyperslab_group = rank; // calc from procid
    nz_hyperslab_width = nz;
    radial_index_bound [0] = 1;
    radial_index_bound [1] = 256;
    theta_index_bound [0] = 1;
    theta_index_bound [1] = ny;
    phi_index_bound [0] = 1;
    phi_index_bound [1] = nz;
    ncycle = 50;
    time = 0.00679921;

    t_c = malloc (sizeof (double) * (nx));
    u_c = malloc (sizeof (double) * (nx));
    v_c = malloc (sizeof (double) * (nx));
    w_c = malloc (sizeof (double) * (nx));
    rho_c = malloc (sizeof (double) * (nx));
    ye_c = malloc (sizeof (double) * (nx));
    nse_c = malloc (sizeof (double) * (nx));
    a_nuc_rep_c = malloc (sizeof (double) * (nx));
    z_nuc_rep_c = malloc (sizeof (double) * (nx));
    be_nuc_rep_c = malloc (sizeof (double) * (nx));
    uburn_c = malloc (sizeof (double) * (nx));
    duesrc = malloc (sizeof (double) * (nx));
    pMD = malloc (sizeof (double) * (nx));
    sMD = malloc (sizeof (double) * (nx));
    dudt_nuc = malloc (sizeof (double) * (nx));
    dudt_nu = malloc (sizeof (double) * (nx));
    grav_x_c = malloc (sizeof (double) * (nx));
    grav_y_c = malloc (sizeof (double) * (nx));
    grav_z_c = malloc (sizeof (double) * (nx));
    agr_e = malloc (sizeof (double) * (nx));
    agr_c = malloc (sizeof (double) * (nx));

    total_written_size += (21 * sizeof (double) * nx);

    for (i = 0; i < nx; i++)
    {
        t_c [i] = rank;
        u_c [i] = rank;
        v_c [i] = rank;
        w_c [i] = rank;
        rho_c [i] = rank;
        ye_c [i] = rank;
        nse_c [i] = rank;
        a_nuc_rep_c [i] = rank;
        z_nuc_rep_c [i] = rank;
        be_nuc_rep_c [i] = rank;
        uburn_c [i] = rank;
        duesrc [i] = rank;
        pMD [i] = rank;
        sMD [i] = rank;
        dudt_nuc [i] = rank;
        dudt_nu [i] = rank;
        grav_x_c [i] = rank;
        grav_y_c [i] = rank;
        grav_z_c [i] = rank;
        agr_e [i] = rank;
        agr_c [i] = rank;
    }

    psi0_c = malloc (sizeof (double) * (nx * nez * nnu * my_j_ray_dim * my_k_ray_dim));
    psi1_e = malloc (sizeof (double) * (nx * nez * nnu * my_j_ray_dim * my_k_ray_dim));
    dnurad = malloc (sizeof (double) * (nx * nez * nnu * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (3 * sizeof (double) * nx * nez * nnu * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < nx * nez * nnu * my_j_ray_dim * my_k_ray_dim; i++)
    {
        psi0_c [i] = rank;
    }

    unukrad = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    nnukrad = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    nu_r = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    nu_rt = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    nu_rho = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    nu_rhot = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    psi0dat = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));
    psi1dat = malloc (sizeof (double) * (nez * nnu * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (8 * sizeof (double) * nez * nnu * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < nez * nnu * my_j_ray_dim * my_k_ray_dim; i++)
    {
        unukrad [i] = rank;
        nnukrad [i] = rank;
        nu_r [i] = rank;
        nu_rt [i] = rank;
        nu_rho [i] = rank;
        nu_rhot [i] = rank;
        psi0dat [i] = rank;
        psi1dat [i] = rank;
    }

    unujrad = malloc (sizeof (double) * (nx * nnu * my_j_ray_dim * my_k_ray_dim));
    nnujrad = malloc (sizeof (double) * (nx * nnu * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (2 * sizeof (double) * nx * nnu * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < nx * nnu * my_j_ray_dim * my_k_ray_dim; i++)
    {
        unujrad [i] = rank;
        nnujrad [i] = rank;
    }

    e_rad = malloc (sizeof (double) * (1));
    elec_rad = malloc (sizeof (double) * (1));
    r_shock = malloc (sizeof (double) * (1));
    r_shock_mn = malloc (sizeof (double) * (1));
    r_shock_mx = malloc (sizeof (double) * (1));
    tau_adv = malloc (sizeof (double) * (1));
    tau_heat_nu = malloc (sizeof (double) * (1));
    tau_heat_nuc = malloc (sizeof (double) * (1));
    r_nse = malloc (sizeof (double) * (1));

    total_written_size += (9 * sizeof (double) * 1);

    e_rad [0] = rank;
    elec_rad [0] = rank;
    r_shock [0] = rank;
    r_shock_mn [0] = rank;
    r_shock_mx [0] = rank;
    tau_adv [0] = rank;
    tau_heat_nu [0] = rank;
    tau_heat_nuc [0] = rank;
    r_nse [0] = rank;

    unurad = malloc (sizeof (double) * (nnu));
    nnurad = malloc (sizeof (double) * (nnu));
    rsphere_mean = malloc (sizeof (double) * (nnu));
    dsphere_mean = malloc (sizeof (double) * (nnu));
    tsphere_mean = malloc (sizeof (double) * (nnu));
    msphere_mean = malloc (sizeof (double) * (nnu));
    esphere_mean = malloc (sizeof (double) * (nnu));

    total_written_size += (7 * sizeof (double) * nnu);

    for (i = 0; i < nnu; i++)
    {
        unurad [i] = rank;
        nnurad [i] = rank;
        rsphere_mean [i] = rank;
        dsphere_mean [i] = rank;
        tsphere_mean [i] = rank;
        msphere_mean [i] = rank;
        esphere_mean [i] = rank;
    }

    e_nu_c_bar = malloc (sizeof (double) * (nx));
    f_nu_e_bar = malloc (sizeof (double) * (nx));

    total_written_size += (2 * sizeof (double) * nx);

    for (i = 0; i < nx; i++)
    {
        e_nu_c_bar [i] = rank;
        f_nu_e_bar [i] = rank;
    }

    xn_c = malloc (sizeof (double) * (nx * nnc * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (1 * sizeof (double) * nx * nnc * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < nx * nnc * my_j_ray_dim * my_k_ray_dim; i++)
    {
        xn_c [i] = rank;
    }

    r_gain = malloc (sizeof (double) * ((nnu + 1) * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (1 * sizeof (double) * (nnu + 1) * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < my_j_ray_dim * my_k_ray_dim; i++)
    {
        r_gain [i] = rank;
    }

    unu_c = malloc (sizeof (double) * (nx * nez * my_j_ray_dim * my_k_ray_dim));
    dunu_c = malloc (sizeof (double) * (nx * nez * my_j_ray_dim * my_k_ray_dim));
    unue_e = malloc (sizeof (double) * (nx * nez * my_j_ray_dim * my_k_ray_dim));
    dunue_e = malloc (sizeof (double) * (nx * nez * my_j_ray_dim * my_k_ray_dim));

    total_written_size += (4 * sizeof (double) * nx * nez * my_j_ray_dim * my_k_ray_dim);

    for (i = 0; i < nx * nez * my_j_ray_dim * my_k_ray_dim; i++)
    {
        unu_c [i] = rank;
        dunu_c [i] = rank;
        unue_e [i] = rank;
        dunue_e [i] = rank;
    }

    total_written_size *= size;

////////////////////////////////////////////
    int dimids [5];

    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime ();

    nc_err = ncmpi_create (MPI_COMM_WORLD, filename, NC_CLOBBER | NC_64BIT_OFFSET, MPI_INFO_NULL, &ncid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to create %s\n", rank, ncmpi_strerror (nc_err));

    nc_err = ncmpi_def_dim (ncid, "nx", nx, &nx_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nx %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nx+1", nx+1, &nx_1_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nx+1 %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "ny", ny, &ny_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim ny %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "ny+1", ny+1, &ny_1_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim ny+1 %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nz", nz, &nz_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nz %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nz+1", nz+1, &nz_1_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nz+1 %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nez", nez, &nez_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nez %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nnu", nnu, &nnu_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nnu %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nnu+1", nnu+1, &nnu_1_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nnu+1 %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "nnc", nnc, &nnc_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim nnc %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "ij_ray_dimid", my_j_ray_dim, &ij_ray_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim ij_ray %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_def_dim (ncid, "ik_ray_dimid", my_k_ray_dim, &ik_ray_dimid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def dim ik_ray %s\n", rank, ncmpi_strerror (nc_err));

    nc_err = ncmpi_def_var (ncid, "myid", NC_INT, 0, 0, &myid_varid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def var myid %s\n", rank, ncmpi_strerror (nc_err));

    dimids [0] = 3;
    nc_err = ncmpi_def_var (ncid, "array_dimensions", NC_INT, 1, dimids, &array_dimensions_varid);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to def var array_dimensions %s\n", rank, ncmpi_strerror (nc_err));

    nc_err = ncmpi_def_var (ncid, "time", NC_DOUBLE, 0, 0, &time_varid);
    nc_err = ncmpi_def_var (ncid, "cycle", NC_INT, 0, 0, &cycle_varid);
    dimids [0] = nx_dimid; dimids [1] = ny_dimid; dimids [2] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "rho_c", NC_DOUBLE, 3, dimids, &rho_c_varid);
    nc_err = ncmpi_def_var (ncid, "t_c", NC_DOUBLE, 3, dimids, &t_c_varid);
    nc_err = ncmpi_def_var (ncid, "ye_c", NC_DOUBLE, 3, dimids, &ye_c_varid);
    nc_err = ncmpi_def_var (ncid, "u_c", NC_DOUBLE, 3, dimids, &u_c_varid);
    nc_err = ncmpi_def_var (ncid, "v_c", NC_DOUBLE, 3, dimids, &v_c_varid);
    nc_err = ncmpi_def_var (ncid, "w_c", NC_DOUBLE, 3, dimids, &w_c_varid);
    nc_err = ncmpi_def_var (ncid, "nse_c", NC_DOUBLE, 3, dimids, &nse_c_varid);
    nc_err = ncmpi_def_var (ncid, "a_nuc_rep_c", NC_DOUBLE, 3, dimids, &a_nuc_rep_c_varid);
    nc_err = ncmpi_def_var (ncid, "z_nuc_rep_c", NC_DOUBLE, 3, dimids, &z_nuc_rep_c_varid);
    nc_err = ncmpi_def_var (ncid, "be_nuc_rep_c", NC_DOUBLE, 3, dimids, &be_nuc_rep_c_varid);
    nc_err = ncmpi_def_var (ncid, "uburn_c", NC_DOUBLE, 3, dimids, &uburn_c_varid);
    nc_err = ncmpi_def_var (ncid, "duesrc", NC_DOUBLE, 3, dimids, &duesrc_varid);
    nc_err = ncmpi_def_var (ncid, "pMD", NC_DOUBLE, 3, dimids, &pMD_varid);
    nc_err = ncmpi_def_var (ncid, "sMD", NC_DOUBLE, 3, dimids, &sMD_varid);
    nc_err = ncmpi_def_var (ncid, "dudt_nuc", NC_DOUBLE, 3, dimids, &dudt_nuc_varid);
    nc_err = ncmpi_def_var (ncid, "dudt_nu", NC_DOUBLE, 3, dimids, &dudt_nu_varid);
    nc_err = ncmpi_def_var (ncid, "grav_x_c", NC_DOUBLE, 3, dimids, &grav_x_c_varid);
    nc_err = ncmpi_def_var (ncid, "grav_y_c", NC_DOUBLE, 3, dimids, &grav_y_c_varid);
    nc_err = ncmpi_def_var (ncid, "grav_z_c", NC_DOUBLE, 3, dimids, &grav_z_c_varid);
    nc_err = ncmpi_def_var (ncid, "agr_e", NC_DOUBLE, 3, dimids, &agr_e_varid);
    nc_err = ncmpi_def_var (ncid, "agr_c", NC_DOUBLE, 3, dimids, &agr_c_varid);

    dimids [0] = nx_dimid; dimids [1] = nez_dimid; dimids [2] = nnu_dimid;
    dimids [3] = ny_dimid; dimids [4] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "psi0_c", NC_DOUBLE, 5, dimids, &psi0_c_varid);
    nc_err = ncmpi_def_var (ncid, "psi1_e", NC_DOUBLE, 5, dimids, &psi1_e_varid);
    nc_err = ncmpi_def_var (ncid, "dnurad", NC_DOUBLE, 5, dimids, &dnurad_varid);

//call adios_write (adios_handle, "j_ray_min", j_ray_min, adios_err)
//call adios_write (adios_handle, "j_ray_min-1", j_ray_min-1, adios_err)
//call adios_write (adios_handle, "k_ray_min", k_ray_min, adios_err)
//call adios_write (adios_handle, "k_ray_min-1", k_ray_min-1, adios_err)

    nc_err = ncmpi_def_var (ncid, "nz_hyperslabs", NC_INT, 0, 0, &nz_hyperslabs_varid);
    nc_err = ncmpi_def_var (ncid, "my_hyperslab_group", NC_INT, 0, 0, &my_hyperslab_group_varid);
    nc_err = ncmpi_def_var (ncid, "nz_hyperslab_width", NC_INT, 0, 0, &nz_hyperslab_width_varid);

    dimids [0] = 2;
    nc_err = ncmpi_def_var (ncid, "radial_index_bound", NC_INT, 1, dimids, &radial_index_bound_varid);
    nc_err = ncmpi_def_var (ncid, "theta_index_bound", NC_INT, 1, dimids, &theta_index_bound_varid);
    nc_err = ncmpi_def_var (ncid, "phi_index_bound", NC_INT, 1, dimids, &phi_index_bound_varid);

    dimids [0] = nx_1_dimid;
    nc_err = ncmpi_def_var (ncid, "x_ef", NC_DOUBLE, 1, dimids, &x_ef_varid);

    dimids [0] = ny_1_dimid;
    nc_err = ncmpi_def_var (ncid, "y_ef", NC_DOUBLE, 1, dimids, &y_ef_varid);

    dimids [0] = nz_1_dimid;
    nc_err = ncmpi_def_var (ncid, "z_ef", NC_DOUBLE, 1, dimids, &z_ef_varid);

    dimids [0] = nx_1_dimid;
    nc_err = ncmpi_def_var (ncid, "x_cf", NC_DOUBLE, 1, dimids, &x_cf_varid);
    nc_err = ncmpi_def_var (ncid, "dx_cf", NC_DOUBLE, 1, dimids, &dx_cf_varid);

    dimids [0] = ny_1_dimid;
    nc_err = ncmpi_def_var (ncid, "y_cf", NC_DOUBLE, 1, dimids, &y_cf_varid);
    nc_err = ncmpi_def_var (ncid, "dy_cf", NC_DOUBLE, 1, dimids, &dy_cf_varid);

    dimids [0] = nz_1_dimid;
    nc_err = ncmpi_def_var (ncid, "z_cf", NC_DOUBLE, 1, dimids, &z_cf_varid);
    nc_err = ncmpi_def_var (ncid, "dz_cf", NC_DOUBLE, 1, dimids, &dz_cf_varid);

    dimids [0] = nez_dimid; dimids [1] = nnu_dimid; dimids [2] = ny_dimid;
    dimids [3] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "unukrad", NC_DOUBLE, 4, dimids, &unukrad_varid);
    nc_err = ncmpi_def_var (ncid, "nu_r", NC_DOUBLE, 4, dimids, &nu_r_varid);
    nc_err = ncmpi_def_var (ncid, "nu_rt", NC_DOUBLE, 4, dimids, &nu_rt_varid);
    nc_err = ncmpi_def_var (ncid, "nu_rho", NC_DOUBLE, 4, dimids, &nu_rho_varid);
    nc_err = ncmpi_def_var (ncid, "nu_rhot", NC_DOUBLE, 4, dimids, &nu_rhot_varid);
    nc_err = ncmpi_def_var (ncid, "psi0dat", NC_DOUBLE, 4, dimids, &psi0dat_varid);
    nc_err = ncmpi_def_var (ncid, "psi1dat", NC_DOUBLE, 4, dimids, &psi1dat_varid);

    dimids [0] = nx_dimid; dimids [1] = nnu_dimid; dimids [2] = ny_dimid;
    dimids [3] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "unujrad", NC_DOUBLE, 4, dimids, &unujrad_varid);
    nc_err = ncmpi_def_var (ncid, "nnujrad", NC_DOUBLE, 4, dimids, &nnujrad_varid);

    dimids [0] = ny_dimid; dimids [1] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "e_rad", NC_DOUBLE, 2, dimids, &e_rad_varid);
    nc_err = ncmpi_def_var (ncid, "elec_rad", NC_DOUBLE, 2, dimids, &elec_rad_varid);
    nc_err = ncmpi_def_var (ncid, "r_shock", NC_DOUBLE, 2, dimids, &r_shock_varid);
    nc_err = ncmpi_def_var (ncid, "r_shock_mn", NC_DOUBLE, 2, dimids, &r_shock_mn_varid);
    nc_err = ncmpi_def_var (ncid, "r_shock_mx", NC_DOUBLE, 2, dimids, &r_shock_mx_varid);
    nc_err = ncmpi_def_var (ncid, "tau_adv", NC_DOUBLE, 2, dimids, &tau_adv_varid);
    nc_err = ncmpi_def_var (ncid, "tau_heat_nu", NC_DOUBLE, 2, dimids, &tau_heat_nu_varid);
    nc_err = ncmpi_def_var (ncid, "tau_heat_nuc", NC_DOUBLE, 2, dimids, &tau_heat_nuc_varid);
    nc_err = ncmpi_def_var (ncid, "r_nse", NC_DOUBLE, 2, dimids, &r_nse_varid);

    dimids [0] = nnu_dimid; dimids [1] = ny_dimid; dimids [2] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "unurad", NC_DOUBLE, 3, dimids, &unurad_varid);
    nc_err = ncmpi_def_var (ncid, "nnurad", NC_DOUBLE, 3, dimids, &nnurad_varid);
    nc_err = ncmpi_def_var (ncid, "rsphere_mean", NC_DOUBLE, 3, dimids, &rsphere_mean_varid);
    nc_err = ncmpi_def_var (ncid, "dsphere_mean", NC_DOUBLE, 3, dimids, &dsphere_mean_varid);
    nc_err = ncmpi_def_var (ncid, "tsphere_mean", NC_DOUBLE, 3, dimids, &tsphere_mean_varid);
    nc_err = ncmpi_def_var (ncid, "msphere_mean", NC_DOUBLE, 3, dimids, &msphere_mean_varid);
    nc_err = ncmpi_def_var (ncid, "esphere_mean", NC_DOUBLE, 3, dimids, &esphere_mean_varid);

    dimids [0] = nx_dimid;
    nc_err = ncmpi_def_var (ncid, "e_nu_c_bar", NC_DOUBLE, 1, dimids, &e_nu_c_bar_varid);
    nc_err = ncmpi_def_var (ncid, "f_nu_e_bar", NC_DOUBLE, 1, dimids, &f_nu_e_bar_varid);

    dimids [0] = nx_dimid; dimids [1] = nnc_dimid; dimids [2] = ny_dimid;
    dimids [3] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "xn_c", NC_DOUBLE, 4, dimids, &xn_c_varid);

    dimids [0] = nnu_1_dimid; dimids [1] = ny_dimid; dimids [2] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "r_gain", NC_DOUBLE, 3, dimids, &r_gain_varid);

    dimids [0] = nx_dimid; dimids [1] = nez_dimid; dimids [2] = ny_dimid;
    dimids [3] = nz_dimid;
    nc_err = ncmpi_def_var (ncid, "unu_c", NC_DOUBLE, 4, dimids, &unu_c_varid);
    nc_err = ncmpi_def_var (ncid, "dunu_c", NC_DOUBLE, 4, dimids, &dunu_c_varid);
    nc_err = ncmpi_def_var (ncid, "unue_e", NC_DOUBLE, 4, dimids, &unue_e_varid);
    nc_err = ncmpi_def_var (ncid, "dunue_e", NC_DOUBLE, 4, dimids, &dunue_e_varid);

    nc_err = ncmpi_enddef (ncid);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    MPI_Offset start [5];
    MPI_Offset count [5];

    ncmpi_begin_indep_data (ncid);
    if (rank == 0)
    {
    nc_err = ncmpi_put_var_int (ncid, myid_varid, &rank);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, array_dimensions_varid, array_dimensions);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var array_dimensions %s\n", rank, ncmpi_strerror (nc_err));

    nc_err = ncmpi_put_var_double (ncid, time_varid, &time);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var time %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, cycle_varid, &ncycle);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var cycle %s\n", rank, ncmpi_strerror (nc_err));
    }
    ncmpi_end_indep_data (ncid);

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nx; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, rho_c_varid, start, count, rho_c);
    nc_err = ncmpi_put_vara_double_all (ncid, t_c_varid, start, count, t_c);
    nc_err = ncmpi_put_vara_double_all (ncid, ye_c_varid, start, count, ye_c);
    nc_err = ncmpi_put_vara_double_all (ncid, u_c_varid, start, count, u_c);
    nc_err = ncmpi_put_vara_double_all (ncid, v_c_varid, start, count, v_c);
    nc_err = ncmpi_put_vara_double_all (ncid, w_c_varid, start, count, w_c);
    nc_err = ncmpi_put_vara_double_all (ncid, nse_c_varid, start, count, nse_c);
    nc_err = ncmpi_put_vara_double_all (ncid, a_nuc_rep_c_varid, start, count, a_nuc_rep_c);
    nc_err = ncmpi_put_vara_double_all (ncid, z_nuc_rep_c_varid, start, count, z_nuc_rep_c);
    nc_err = ncmpi_put_vara_double_all (ncid, be_nuc_rep_c_varid, start, count, be_nuc_rep_c);
    nc_err = ncmpi_put_vara_double_all (ncid, uburn_c_varid, start, count, uburn_c);
    nc_err = ncmpi_put_vara_double_all (ncid, duesrc_varid, start, count, duesrc);
    nc_err = ncmpi_put_vara_double_all (ncid, pMD_varid, start, count, pMD);
    nc_err = ncmpi_put_vara_double_all (ncid, sMD_varid, start, count, sMD);
    nc_err = ncmpi_put_vara_double_all (ncid, dudt_nuc_varid, start, count, dudt_nuc);
    nc_err = ncmpi_put_vara_double_all (ncid, dudt_nu_varid, start, count, dudt_nu);
    nc_err = ncmpi_put_vara_double_all (ncid, grav_x_c_varid, start, count, grav_x_c);
    nc_err = ncmpi_put_vara_double_all (ncid, grav_y_c_varid, start, count, grav_y_c);
    nc_err = ncmpi_put_vara_double_all (ncid, grav_z_c_varid, start, count, grav_z_c);
    nc_err = ncmpi_put_vara_double_all (ncid, agr_e_varid, start, count, agr_e);
    nc_err = ncmpi_put_vara_double_all (ncid, agr_c_varid, start, count, agr_c);

    start [0] = 0; start [1] = 0; start [2] = 0; start [3] = j_ray_min-1;
    start [4] = k_ray_min-1;
    count [0] = nx; count [1] = nez; count [2] = nnu; count [3] = my_j_ray_dim;
    count [4] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, psi0_c_varid, start, count, psi0_c);
    nc_err = ncmpi_put_vara_double_all (ncid, psi1_e_varid, start, count, psi1_e);
    nc_err = ncmpi_put_vara_double_all (ncid, dnurad_varid, start, count, dnurad);

//***************************************************
    ncmpi_begin_indep_data (ncid);
    if (rank == 0)
    {
    start [0] = 0; count [0] = 1;
    nc_err = ncmpi_put_var_int (ncid, nz_hyperslabs_varid, &nz_hyperslabs);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, my_hyperslab_group_varid, &my_hyperslab_group);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, nz_hyperslab_width_varid, &nz_hyperslab_width);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));

    start [0] = 0; count [0] = 2;
    nc_err = ncmpi_put_var_int (ncid, radial_index_bound_varid, radial_index_bound);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, theta_index_bound_varid, theta_index_bound);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    nc_err = ncmpi_put_var_int (ncid, phi_index_bound_varid, phi_index_bound);
    if (nc_err != NC_NOERR) printf ("rank: %02d failure to put var myid %s\n", rank, ncmpi_strerror (nc_err));
    }
    ncmpi_end_indep_data (ncid);
//***************************************************

    start [0] = 0; count [0] = nx+1;
    nc_err = ncmpi_put_vara_double_all (ncid, x_ef_varid, start, count, x_ef);

    start [0] = 0; count [0] = ny+1;
    nc_err = ncmpi_put_vara_double_all (ncid, y_ef_varid, start, count, y_ef);

    start [0] = 0; count [0] = nz+1;
    nc_err = ncmpi_put_vara_double_all (ncid, z_ef_varid, start, count, z_ef);

    start [0] = 0; count [0] = nx;
    nc_err = ncmpi_put_vara_double_all (ncid, x_cf_varid, start, count, x_cf);
    nc_err = ncmpi_put_vara_double_all (ncid, dx_cf_varid, start, count, dx_cf);

    start [0] = 0; count [0] = ny;
    nc_err = ncmpi_put_vara_double_all (ncid, y_cf_varid, start, count, y_cf);
    nc_err = ncmpi_put_vara_double_all (ncid, dy_cf_varid, start, count, dy_cf);

    start [0] = 0; count [0] = nz;
    nc_err = ncmpi_put_vara_double_all (ncid, z_cf_varid, start, count, z_cf);
    nc_err = ncmpi_put_vara_double_all (ncid, dz_cf_varid, start, count, dz_cf);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nez; count [1] = nnu; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, unukrad_varid, start, count, unukrad);
    nc_err = ncmpi_put_vara_double_all (ncid, nu_r_varid, start, count, nu_r);
    nc_err = ncmpi_put_vara_double_all (ncid, nu_rt_varid, start, count, nu_rt);
    nc_err = ncmpi_put_vara_double_all (ncid, nu_rho_varid, start, count, nu_rho);
    nc_err = ncmpi_put_vara_double_all (ncid, nu_rhot_varid, start, count, nu_rhot);
    nc_err = ncmpi_put_vara_double_all (ncid, psi0dat_varid, start, count, psi0dat);
    nc_err = ncmpi_put_vara_double_all (ncid, psi1dat_varid, start, count, psi1dat);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nnu; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, unujrad_varid, start, count, unujrad);
    nc_err = ncmpi_put_vara_double_all (ncid, nnujrad_varid, start, count, nnujrad);

    start [0] = j_ray_min-1; k_ray_min-1;
    count [0] = my_j_ray_dim; count [1] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, e_rad_varid, start, count, e_rad);
    nc_err = ncmpi_put_vara_double_all (ncid, elec_rad_varid, start, count, elec_rad);
    nc_err = ncmpi_put_vara_double_all (ncid, r_shock_varid, start, count, r_shock);
    nc_err = ncmpi_put_vara_double_all (ncid, r_shock_mn_varid, start, count, r_shock_mn);
    nc_err = ncmpi_put_vara_double_all (ncid, r_shock_mx_varid, start, count, r_shock_mx);
    nc_err = ncmpi_put_vara_double_all (ncid, tau_adv_varid, start, count, tau_adv);
    nc_err = ncmpi_put_vara_double_all (ncid, tau_heat_nu_varid, start, count, tau_heat_nu);
    nc_err = ncmpi_put_vara_double_all (ncid, tau_heat_nuc_varid, start, count, tau_heat_nuc);
    nc_err = ncmpi_put_vara_double_all (ncid, r_nse_varid, start, count, r_nse);

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nnu; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, unurad_varid, start, count, unurad);
    nc_err = ncmpi_put_vara_double_all (ncid, nnurad_varid, start, count, nnurad);
    nc_err = ncmpi_put_vara_double_all (ncid, rsphere_mean_varid, start, count, rsphere_mean);
    nc_err = ncmpi_put_vara_double_all (ncid, dsphere_mean_varid, start, count, dsphere_mean);
    nc_err = ncmpi_put_vara_double_all (ncid, tsphere_mean_varid, start, count, tsphere_mean);
    nc_err = ncmpi_put_vara_double_all (ncid, msphere_mean_varid, start, count, msphere_mean);
    nc_err = ncmpi_put_vara_double_all (ncid, esphere_mean_varid, start, count, esphere_mean);

    ncmpi_begin_indep_data (ncid);
    if (rank == 0)
    {
    start [0] = 0;
    count [0] = nx;
    nc_err = ncmpi_put_vara_double (ncid, e_nu_c_bar_varid, start, count, e_nu_c_bar);
    nc_err = ncmpi_put_vara_double (ncid, f_nu_e_bar_varid, start, count, f_nu_e_bar);
    }
    ncmpi_end_indep_data (ncid);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nnc; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, xn_c_varid, start, count, xn_c);

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nnu+1; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, r_gain_varid, start, count, r_gain);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nez; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    nc_err = ncmpi_put_vara_double_all (ncid, unu_c_varid, start, count, unu_c);
    nc_err = ncmpi_put_vara_double_all (ncid, dunu_c_varid, start, count, dunu_c);
    nc_err = ncmpi_put_vara_double_all (ncid, unue_e_varid, start, count, unue_e);
    nc_err = ncmpi_put_vara_double_all (ncid, dunue_e_varid, start, count, dunue_e);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    nc_err = ncmpi_close (ncid);
    //if (nc_err != NC_NOERR) call handle_err (nc_err)

    MPI_Barrier (MPI_COMM_WORLD);
    end_time = MPI_Wtime ();

    t_time = end_time - start_time;
    sz = total_written_size / (1024.0 * 1024.0 * 1024.0);
    gps = sz / t_time;
    if (rank == 0)
        printf ("%s %d %lf %lf %lf\n", filename, size, sz, t_time, gps);

    MPI_Finalize ();

    return 0;
}
