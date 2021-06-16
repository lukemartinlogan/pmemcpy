
# PMEMCPY

A simple, lightweight, and portable I/O library for interfacing with persistent
memory devices.

## Dependencies

* cereal, msgpack, boost, capnproto
* pmdk
* netcdf
* adios

## Install

```{bash}
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
cmake ../ -DCMAKE_INSTALL_PREFIX=`scspkg pkg-root pmemcpy`
```