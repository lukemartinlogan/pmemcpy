# e2e-hpdc2011
IO testing harness for my HPDC2011 paper
load these modules:

module load netcdf-hdf5parallel/4.1.1.0
module load p-netcdf/1.1.1

ADIOS env vars to setup:
export ADIOS_DIR=/ccs/home/ge1/work/jaguar
export ADIOS_INC=-I/ccs/home/ge1/work/jaguar/include
export ADIOSREAD_CLIB="-L/ccs/home/ge1/work/jaguar/lib -ladiosread"
export ADIOSREAD_FLIB="-L/ccs/home/ge1/work/jaguar/lib -ladiosreadf"
export ADIOS_CLIB="-L/ccs/home/ge1/work/jaguar/lib -ladios -lmxml"
export ADIOS_FLIB="-L/ccs/home/ge1/work/jaguar/lib -ladiosf -lmxml"
