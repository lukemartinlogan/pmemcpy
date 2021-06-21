cd ${HOME}
git clone git://github.com/spack/spack.git
cd spack
git checkout releases/v0.16
echo ". `pwd`/share/spack/setup-env.sh" >> ~/.bashrc
source ~/.bashrc

sudo apt install linux-headers-`uname -r`
sudo apt install cmake
spack install netcdf-c adios parallel-netcdf capnproto msgpack-c boost cereal pmdk
echo "spack load netcdf-c adios parallel-netcdf capnproto msgpack-c boost cereal pmdk" >> ~/.bashrc
source ~/.bashrc