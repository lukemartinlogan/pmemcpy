#include <hdf5.h>
int main (int argc, char **argv) {
  int nprocs, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  hid_t file_id, dset_id;
  hid_t filespace, memspace;
  hsize_t	count = 100;
  hsize_t offset = rank*100;
  hsize_t dimsf = nprocs*100;
  hid_t	plist_id;
  herr_t	status;
  char *path = argv[1];
  int data[100];

  plist_id = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(plist_id,
    MPI_COMM_WORLD, MPI_INFO_NULL);
  file_id = H5Fcreate(path,
    H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
  H5Pclose(plist_id);

  filespace = H5Screate_simple(1, &dimsf, NULL);
  dset_id = H5Dcreate(file_id, "dataset",
    H5T_NATIVE_INT, filespace, H5P_DEFAULT,
    H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(filespace);
  memspace = H5Screate_simple(1, &count, NULL);
  filespace = H5Dget_space(dset_id);
  H5Sselect_hyperslab(filespace,
    H5S_SELECT_SET, &offset,
    NULL, &count, NULL);

  plist_id = H5Pcreate(H5P_DATASET_XFER);
  status = H5Dwrite(dset_id, H5T_NATIVE_INT,
    memspace, filespace, plist_id, data);

  H5Dclose(dset_id);
  H5Sclose(filespace);
  H5Sclose(memspace);
  H5Pclose(plist_id);
  H5Fclose(file_id);
  MPI_Finalize();
  return 0;
}
