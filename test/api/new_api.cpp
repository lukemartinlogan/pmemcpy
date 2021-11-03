#include <pmemcpy/pmemcpy.hpp>
pmemcpy::PMEM pmem(pmemcpy::SerializerType default_);
pmem.mmap(std::string filename, int comm);
pmem.munmap();

pmem.store<T>(std::string id, T &data,
  pmemcpy::SerializerType s = Default);
pmem.alloc<T>(std::string id,
  int ndims, size_t *dims);
pmem.store<T>(std::string id, T *data,
  int ndims, size_t *offsets, size_t *dimspp,
  pmemcpy::SerializerType s = Default);

pmem.load<T>(std::string id);
pmem.load<T>(std::string id, T &num);
pmem.load<T>(std::string id, T *data,
  int ndims, size_t *offsets, size_t *dimspp);
pmem.load_dims(std::string id,
  int *ndims, size_t *dim);
