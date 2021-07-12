
# PMEMCPY

A simple, lightweight, and portable I/O library for interfacing with persistent
memory devices. This is a mostly header-only library.

## Dependencies

* boost, capnproto
* pmdk

```bash
spack install boost capnproto pmdk
```

## Install

```bash
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
```

To build with tests (requires netcdf, adios, and pnetcdf):
```bash
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TESTS=ON
```

## Usage

Examples can be seen under test/unit/test_pmem and test/performance/e2e/write_pmemcpy_omp and read_pmemcpy_omp.
test/api was used to make pseudocode for the paper, and differs slightly from the actual work.

### APIs
```C
#include <pmemcpy/memcpy.h>

pmemcpy::PMEM pmem(StorageType storage, SerializerType serializer, bool use_mmap);
pmem.mmap(std::string path, size_t size);
pmem.munmap()

pmem.store<T>(std::string id, T &data);
pmem.store<T>(std::string id, T *data, pmemcpy::Dimensions dims);

pmem.load<T>(std::string id, T &data);
pmem.load<T>(std::string id, T *data, pmemcpy::Dimensions dims);

pmem.free(std::string id);
```

* pmem.mmap: Allocates "size" bytes from the file at "path". Creates the file with this size if it does not exist.
* pmem.munmap: Unmap the memory region. Automatically called in destructor.

### Serializer Types
Different serialization methods can be used to serialize the data in PMEM:

```C
pmemcpy::SerializerType::CAPNPROTO
pmemcpy::SerializerType::CAPNPROTO_NOCOMPRESS
pmemcpy::SerializerType::NO_SERIALIZER
```

By default, CapnProto will encode data using a simple deduplication algorithm. This can be disabled using the CAPNPROTO_NOCOMPRESS option.

### Storage Types
The act of placing data into the PMEM can be done using either POSIX or PMDK.
The POSIX method will assume that a directory is passed to pmem.mmap, and each
variable created using pmem.store will be a file in this directory. You cannot
use "/" in the id name.

```C
pmemcpy::StorageType::POSIX
pmemcpy::StorageType::PMDK
pmemcpy::StorageType::PMDK_FS
```

* PMDK uses a hashtable with chaining to store data structures. It requires the user to mmap enough data to store all data structures in pmem.mmap().
* PMDK_FS and POSIX creates a file for each data structure. POSIX uses mmap without MAP_SYNC, PMDK_FS uses MAP_SYNC.

### Error Handling

We provide a wrapper around try/catch to help with error handling.
Variables defined within this region cannot be accessed after PMEMCPY_ERROR_HANDLE_END has been called.
They will be out of scope.
```C
PMEMCPY_ERROR_HANDLE_START()
//PMEMCPY code...
PMEMCPY_ERROR_HANDLE_END()
```

## Limitations

* pmem.mmap must be called with the "size" parameter in order to create the pool for the PMDK storage type. PMDK pools cannot be extended whence allocated. However, the size parameter is not needed for the POSIX storage type; size is determined automatically.
* pmem.mmap cannot be called in parallel to the same path when using PMDK. In other words, when using PMDK with MPI, you must either share the PMEM object among all processes or use file-per-processs. The POSIX storage type does not have this limitation.
* For writes, CapnProto requires the memory region where data gets serialized to be zeroed using memset, which is fairly expensive. Reads are fine.
* CapnProto can only serialize primitive types (char, int, float, double) and C-style arrays of these types. Slightly more work would have to be done in order to incorporate STL or custom structured types.
* PMDK adds a fair amount of overhead for crash consistency concerns.
