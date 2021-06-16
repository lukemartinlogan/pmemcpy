adios_groupsize = 4 \
                + 4 \
                + 4 \
                + 4 \
                + 8 \
                + 8 \
                + 8 \
                + 8 \
                + 8 * (nparam) * (l_ntracke) \
                + 8 * (nparam) * (l_ntracki);
adios_group_size (adios_handle, adios_groupsize, &adios_totalsize);
adios_write (adios_handle, "mype", &mype);
adios_write (adios_handle, "nparam", &nparam);
adios_write (adios_handle, "l_ntracke", &l_ntracke);
adios_write (adios_handle, "l_ntracki", &l_ntracki);
adios_write (adios_handle, "ntracke", &ntracke);
adios_write (adios_handle, "ntracki", &ntracki);
adios_write (adios_handle, "offset_e", &offset_e);
adios_write (adios_handle, "offset_i", &offset_i);
adios_write (adios_handle, "ptrackede", ptrackede);
adios_write (adios_handle, "ptrackedi", ptrackedi);
