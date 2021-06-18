# e2e-hpdc2011

sudo mount -t tmpfs -o size=4000m swap '/home/lukemartinlogan/tmpfs'

/home/lukemartinlogan/spack/opt/spack/linux-linuxmint20-zen2/gcc-9.3.0/adios-1.13.1-rjcwbpi6fpihml3c4r2mxxejra4zvflq/bin/gpp.py 1d.xml
/home/lukemartinlogan/spack/opt/spack/linux-linuxmint20-zen2/gcc-9.3.0/adios-1.13.1-rjcwbpi6fpihml3c4r2mxxejra4zvflq/bin/gpp.py 3d.xml

cd test/performance/e2e-1
mpirun -n 48 --use-hwthread-cpus ./write_3d_bp /home/cc/tmpfs/test /home/cc/pmemcpy/test/performance/e2e-1/3d/3d.xml 2 3 8 256 256 256
rm -r ~/tmpfs/*
mpirun -n 48 --use-hwthread-cpus ./write_3d_nc4 /home/cc/tmpfs/test 2 3 8 256 256 256
rm -r ~/tmpfs/*
mpirun -n 48 --use-hwthread-cpus ./write_3d_nc5 /home/cc/tmpfs/test 2 3 8 256 256 256
rm -r ~/tmpfs/*
./write_3d_pmemcpy_omp /home/cc/tmpfs/test 2 3 8 256 256 256 48
rm -r ~/tmpfs/*

mpirun -n 24 ./write_3d_bp /home/cc/tmpfs/test /home/cc/pmemcpy/test/performance/e2e-1/3d/3d.xml 2 3 4 256 256 256
rm -r ~/tmpfs/*
mpirun -n 24 ./write_3d_nc4 /home/cc/tmpfs/test 2 3 4 256 256 256
rm -r ~/tmpfs/*
mpirun -n 24 ./write_3d_nc5 /home/cc/tmpfs/test 2 3 4 256 256 256
rm -r ~/tmpfs/*
./write_3d_pmemcpy_omp /home/cc/tmpfs/test 2 3 4 256 256 256 24
rm -r ~/tmpfs/*