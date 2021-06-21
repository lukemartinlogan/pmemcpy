cd ${HOME}
git clone git://github.com/spack/spack.git
cd spack
git checkout releases/v0.16
echo ". `pwd`/share/spack/setup-env.sh" >> ~/.bashrc
source ~/.bashrc

spack install netcdf-c adios parallel-netcdf capnproto msgpack-c boost cereal
echo "spack load netcdf-c adios parallel-netcdf capnproto msgpack-c boost cereal" >> ~/.bashrc
source ~/.bashrc
