#include <pmemcpy/pmemcpy.hpp>
pmemcpy::PMEM pmem;
pmem.mmap(std::string filename, int comm);
pmem.munmap();

pmem.store<T>(std::string id, T &data);
pmem.alloc<T>(std::string id,
  int ndims, size_t *dims);
pmem.alloc<T>(std::string id,
  pmemcpy::Dimensions dims);
pmem.store<T>(std::string id, T *data,
  int ndims, size_t *offsets, size_t *dimspp);

pmem.load<T>(std::string id);
pmem.load<T>(std::string id, T &num);
pmem.load<T>(std::string id, T *data,
  int ndims, size_t *offsets, size_t *dimspp);
pmem.load_dims(std::string id,
  int *ndims, size_t *dim);