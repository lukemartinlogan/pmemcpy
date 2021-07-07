
#LOGIN to Chameleon
ssh cc@129.114.108.56

#Install spack and pmem dependencies
cd ${HOME}
git clone git://github.com/spack/spack.git
cd spack
git checkout releases/v0.16
echo ". `pwd`/share/spack/setup-env.sh" >> ~/.bashrc
source ~/.bashrc

#Install pmem dependencies
sudo apt install cmake
spack install netcdf-c parallel-netcdf adios msgpack-c capnproto pmdk boost cereal
echo "spack load netcdf-c parallel-netcdf adios msgpack-c capnproto pmdk boost cereal" >> ~/.bashrc
source ~/.bashrc

#Emulate pmem
sudo nano /etc/default/grub
GRUB_CMDLINE_LINUX="memmap=60G!10G"
sudo update-grub2

#COPY pmemcpy to Chameleon
cd /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/cmake-build-debug
ROOT_DIR=../
tar -czf pmemcpy.tar.gz ${ROOT_DIR}/include ${ROOT_DIR}/src ${ROOT_DIR}/test ${ROOT_DIR}/scripts ${ROOT_DIR}/CMakeLists.txt
scp pmemcpy.tar.gz cc@129.114.108.56:~/

#DECOMPRESS pmemcpy on Chameleon
mkdir ${HOME}/pmemcpy
tar -xzf ${HOME}/pmemcpy.tar.gz -C ${HOME}/pmemcpy

#BUILD pmemcpy
cd ${HOME}/pmemcpy
mkdir cmake-build-debug
cd cmake-build-debug
cmake ../
make -j8

#MAKE filesystem
mkdir ~/tmpfs
sudo mkfs.ext4 /dev/pmem0
sudo mount -o dax /dev/pmem0 /home/cc/tmpfs
#sudo mount -t pmemulator-ext4 -o dax /dev/pmem0 ~/tmpfs
sudo chown -R cc /home/cc/tmpfs
sudo umount /home/cc/tmpfs

#ALL TESTS
bash ~/pmemcpy/scripts/tests.sh
scp cc@129.114.108.56:~/pmemcpy/cmake-build-debug/log.txt log.txt
scp -i ~/.ssh/id_rsa_pass ../scripts/tests.sh cc@129.114.108.56:/home/cc/pmemcpy/scripts/tests.sh
scp -i ~/.ssh/id_rsa_pass /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/test/performance/e2e/3d/3d.xml cc@129.114.108.56:/home/cc/pmemcpy/test/performance/e2e/3d/3d.xml

#
import pandas as pd
df = pd.read_csv("log.csv")
cols = list(df.columns)
cols.remove("time")
df = df.groupby(cols).mean().reset_index()
df.to_csv("averages2.csv")