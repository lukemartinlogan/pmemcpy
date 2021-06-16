adios_groupsize = 4 \
                + 4 \
                + 4 \
                + 4 \
                + 4 \
                + 4 \
                + 4 \
                + 4 \
                + 4 \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz) \
                + 8 * (ndx) * (ndy) * (ndz);
adios_group_size (adios_handle, adios_groupsize, &adios_totalsize);
adios_write (adios_handle, "nx", &nx);
adios_write (adios_handle, "ny", &ny);
adios_write (adios_handle, "nz", &nz);
adios_write (adios_handle, "offx", &offx);
adios_write (adios_handle, "offy", &offy);
adios_write (adios_handle, "offz", &offz);
adios_write (adios_handle, "ndx", &ndx);
adios_write (adios_handle, "ndy", &ndy);
adios_write (adios_handle, "ndz", &ndz);
adios_write (adios_handle, "A", A);
adios_write (adios_handle, "B", B);
adios_write (adios_handle, "C", C);
adios_write (adios_handle, "D", D);
adios_write (adios_handle, "E", E);
adios_write (adios_handle, "F", F);
adios_write (adios_handle, "G", G);
adios_write (adios_handle, "H", H);
adios_write (adios_handle, "I", I);
adios_write (adios_handle, "J", J);
