# e2e-hpdc2011

sudo mount -t tmpfs -o size=4000m swap '/home/lukemartinlogan/tmpfs'

/home/lukemartinlogan/spack/opt/spack/linux-linuxmint20-zen2/gcc-9.3.0/adios-1.13.1-rjcwbpi6fpihml3c4r2mxxejra4zvflq/bin/gpp.py 1d.xml
/home/lukemartinlogan/spack/opt/spack/linux-linuxmint20-zen2/gcc-9.3.0/adios-1.13.1-rjcwbpi6fpihml3c4r2mxxejra4zvflq/bin/gpp.py 3d.xml

scp -r include/* cc@129.114.109.179:"~/pmemcpy/include"
scp -r src/* cc@129.114.109.179:"~/pmemcpy/src"
scp -r scripts/* cc@129.114.109.179:"~/pmemcpy/scripts"
scp -r test/* cc@129.114.109.179:"~/pmemcpy/test"
scp CMakeLists.txt cc@129.114.109.179:"~/CMakeLists.txt"

scp -r test/performance/e2e-1/* cc@129.114.109.179:"~/pmemcpy/test/performance/e2e/*"
scp -r test/performance/pmemulator/* cc@129.114.109.179:"~/pmemcpy/test/performance/pmemulator/*"
scp -r scripts/tests.sh cc@129.114.109.179:"~/pmemcpy/scripts/test.sh"

PMEM_DIR=/home/cc/tmpfs
EXEC_DIR=~/pmemcpy/cmake-build-debug/test/performance/e2e
mpirun -n 8 ${EXEC_DIR}/write_3d_nc5 ${PMEM_DIR}/test 2 2 2 128 128 128