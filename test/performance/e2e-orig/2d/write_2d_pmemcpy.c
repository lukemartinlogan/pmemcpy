#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pmemcpy/memcpy.h>
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

    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_HASHTABLE, pmemcpy::SerializerType::MSGPACK);
    pmem.mmap(filename, 100*(1<<20));
    if (rank == 0) {
        pmem.store<int>("myid", rank);
        pmem.store<long>("nx", nx);
        pmem.store<long>("ny", ny);
        pmem.store<long>("nz", nz);
        pmem.store<long>("nez", nz);
        pmem.store<long>("nnu", nnu);
        pmem.store<long>("nnc", nnc);
        pmem.store<long>("nnc", nnc);
        pmem.store<long>("array_dimensions", 3);
        pmem.store<double>("time", &time);
        pmem.store<int>("cycle", &cycle);
        pmem.store<int>("cycle", &cycle);
        pmem.alloc<double>("rho_c", nx, ny, nz);
        pmem.alloc<double>("t_c", nx, ny, nz);
        pmem.alloc<double>("ye_c", nx, ny, nz);
        pmem.alloc<double>("u_c", nx, ny, nz);
        pmem.alloc<double>("v_c", nx, ny, nz);
        pmem.alloc<double>("w_c", nx, ny, nz);
        pmem.alloc<double>("nse_c", nx, ny, nz);
        pmem.alloc<double>("a_nuc_rep_c", nx, ny, nz);
        pmem.alloc<double>("z_nuc_rep_c", nx, ny, nz);
        pmem.alloc<double>("be_nuc_rep_c", nx, ny, nz);
        pmem.alloc<double>("uburn_c", nx, ny, nz);
        pmem.alloc<double>("duesrc", nx, ny, nz);
        pmem.alloc<double>("pMD", nx, ny, nz);
        pmem.alloc<double>("sMD", nx, ny, nz);
        pmem.alloc<double>("dudt_nuc", nx, ny, nz);
        pmem.alloc<double>("dudt_nu", nx, ny, nz);
        pmem.alloc<double>("grav_x_c", nx, ny, nz);
        pmem.alloc<double>("grav_y_c", nx, ny, nz);
        pmem.alloc<double>("grav_z_c", nx, ny, nz);
        pmem.alloc<double>("agr_e", nx, ny, nz);
        pmem.alloc<double>("agr_c", nx, ny, nz);
        pmem.alloc<double>("psi0_c", nx, nez, nnu, ny, nz);
        pmem.alloc<double>("psi1_e", nx, nez, nnu, ny, nz);
        pmem.alloc<double>("dnurad", nx, nez, nnu, ny, nz);
        pmem.store<int>("nz_hyperslabs", nz_hyperslabs);
        pmem.store<int>("my_hyperslab_group", my_hyperslab_group);
        pmem.store<int>("nz_hyperslab_width", nz_hyperslab_width);
        pmem.store<int>("radial_index_bound", radial_index_bound, 2);
        pmem.store<int>("theta_index_bound", theta_index_bound, 2);
        pmem.store<int>("phi_index_bound", phi_index_bound, 2);
        pmem.store<int>("x_ef", x_ef, nx+1);
        pmem.store<int>("y_ef", y_ef, ny+1);
        pmem.store<int>("z_ef", z_ef, nz+1);
        pmem.store<double>("x_cf", x_cf, nx);
        pmem.store<double>("dx_cf", dx_cf, nx);
        pmem.store<double>("y_cf", y_cf, ny);
        pmem.store<double>("dy_cf", dy_cf, ny);
        pmem.store<double>("z_cf", z_cf, nz);
        pmem.store<double>("dz_cf", dz_cf, nz);
        pmem.alloc<double>("unukrad", nez, nnu, ny, nz);
        pmem.alloc<double>("nu_r", nez, nnu, ny, nz);
        pmem.alloc<double>("nu_rt", nez, nnu, ny, nz);
        pmem.alloc<double>("nu_rho", nez, nnu, ny, nz);
        pmem.alloc<double>("nu_rhot", nez, nnu, ny, nz);
        pmem.alloc<double>("psi0dat", nez, nnu, ny, nz);
        pmem.alloc<double>("psi1dat", nez, nnu, ny, nz);
        pmem.alloc<double>("unujrad", nx, nnu, ny, nz);
        pmem.alloc<double>("nnujrad", nx, nnu, ny, nz);
        pmem.alloc<double>("e_rad", ny, nz);
        pmem.alloc<double>("elec_rad", ny, nz);
        pmem.alloc<double>("r_shock", ny, nz);
        pmem.alloc<double>("r_shock_mn", ny, nz);
        pmem.alloc<double>("r_shock_mx", ny, nz);
        pmem.alloc<double>("tau_adv", ny, nz);
        pmem.alloc<double>("tau_heat_nu", ny, nz);
        pmem.alloc<double>("r_nse", ny, nz);
        pmem.alloc<double>("unurad", nnu, ny, nz);
        pmem.alloc<double>("nnurad", nnu, ny, nz);
        pmem.alloc<double>("rsphere_mean", nnu, ny, nz);
        pmem.alloc<double>("dsphere_mean", nnu, ny, nz);
        pmem.alloc<double>("tsphere_mean", nnu, ny, nz);
        pmem.alloc<double>("msphere_mean", nnu, ny, nz);
        pmem.alloc<double>("esphere_mean", nnu, ny, nz);
        pmem.store<double>("e_nu_c_bar", e_nu_c_bar, e_nu_c_bar, nx);
        pmem.store<double>("f_nu_e_bar", f_nu_e_bar, f_nu_e_bar, nx);
        pmem.alloc<double>("xn_c", nx, nnc, ny, nz);
        pmem.alloc<double>("r_gain", nnu, ny, nz);
        pmem.alloc<double>("unu_c", nx, nez, ny, nz);
        pmem.alloc<double>("dunu_c", nx, nez, ny, nz);
        pmem.alloc<double>("unue_e", nx, nez, ny, nz);
        pmem.alloc<double>("dunue_e", nx, nez, ny, nz);
    }

    MPI_Offset start [5];
    MPI_Offset count [5];

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nx; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    pmem.store<double>("rho_c", rho_c, start, count);
    pmem.store<double>("t_c", t_c, start, count);
    pmem.store<double>("ye_c", ye_c, start, count);
    pmem.store<double>("u_c", u_c, start, count);
    pmem.store<double>("v_c", v_c, start, count);
    pmem.store<double>("w_c", w_c, start, count);
    pmem.store<double>("nse_c", nse_c, start, count);
    pmem.store<double>("a_nuc_rep_c", a_nuc_rep_c, start, count);
    pmem.store<double>("z_nuc_rep_c", z_nuc_rep_c, start, count);
    pmem.store<double>("be_nuc_rep_c", be_nuc_rep_c, start, count);
    pmem.store<double>("uburn_c", uburn_c, start, count);
    pmem.store<double>("duesrc", duesrc, start, count);
    pmem.store<double>("pMD", pMD, start, count);
    pmem.store<double>("sMD", sMD, start, count);
    pmem.store<double>("dudt_nuc", dudt_nuc, start, count);
    pmem.store<double>("dudt_nu", dudt_nu, start, count);
    pmem.store<double>("grav_x_c", grav_x_c, start, count);
    pmem.store<double>("grav_y_c", grav_y_c, start, count);
    pmem.store<double>("grav_z_c", grav_z_c, start, count);
    pmem.store<double>("agr_e", agr_e, start, count);
    pmem.store<double>("agr_c", agr_c, start, count);

    start [0] = 0; start [1] = 0; start [2] = 0; start [3] = j_ray_min-1; start [4] = k_ray_min-1;
    count [0] = nx; count [1] = nez; count [2] = nnu; count [3] = my_j_ray_dim; count [4] = my_k_ray_dim;
    pmem.store<double>("psi0_c", psi0_c, start, count);
    pmem.store<double>("psi1_e", psi1_e, start, count);
    pmem.store<double>("dnurad", dnurad, start, count);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nez; count [1] = nnu; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    pmem.store<double>("unukrad", unukrad, start, count);
    pmem.store<double>("nu_r", unukrad, start, count);
    pmem.store<double>("nu_rt", unukrad, start, count);
    pmem.store<double>("nu_rho", unukrad, start, count);
    pmem.store<double>("nu_rhot", unukrad, start, count);
    pmem.store<double>("psi0dat", unukrad, start, count);
    pmem.store<double>("psi1dat", unukrad, start, count);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nnu; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    pmem.store<double>("unujrad", unujrad, start, count);
    pmem.store<double>("nnujrad", nnujrad, start, count);

    start [0] = j_ray_min-1; k_ray_min-1;
    count [0] = my_j_ray_dim; count [1] = my_k_ray_dim; 
    pmem.store<double>("e_rad", e_rad, start, count);
    pmem.store<double>("elec_rad", elec_rad, start, count);
    pmem.store<double>("r_shock", r_shock, start, count);
    pmem.store<double>("r_shock_mn", r_shock_mn, start, count);
    pmem.store<double>("r_shock_mx", r_shock_mx, start, count);
    pmem.store<double>("tau_adv", tau_adv, start, count);
    pmem.store<double>("tau_heat_nu", tau_heat_nu, start, count);
    pmem.store<double>("r_nse", r_nse, start, count);

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nnu; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    pmem.store<double>("unurad", unurad, start, count);
    pmem.store<double>("nnurad", nnurad, start, count);
    pmem.store<double>("rsphere_mean", rsphere_mean, start, count);
    pmem.store<double>("dsphere_mean", dsphere_mean, start, count);
    pmem.store<double>("tsphere_mean", tsphere_mean, start, count);
    pmem.store<double>("msphere_mean", msphere_mean, start, count);
    pmem.store<double>("esphere_mean", esphere_mean, start, count);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nnc; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    pmem.store<double>("xn_c", xn_c, start, count);

    start [0] = 0; start [1] = j_ray_min-1; start [2] = k_ray_min-1;
    count [0] = nnu+1; count [1] = my_j_ray_dim; count [2] = my_k_ray_dim;
    pmem.store<double>("r_gain", r_gain, start, count);

    start [0] = 0; start [1] = 0; start [2] = j_ray_min-1; start [3] = k_ray_min-1;
    count [0] = nx; count [1] = nez; count [2] = my_j_ray_dim; count [3] = my_k_ray_dim;
    pmem.store<double>("unu_c", unu_c, start, count);
    pmem.store<double>("dunu_c", dunu_c, start, count);
    pmem.store<double>("unue_e", unue_e, start, count);
    pmem.store<double>("dunue_e", dunue_e, start, count);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
