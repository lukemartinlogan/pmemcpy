
#LOGIN to Chameleon
IP=129.114.108.10
ssh cc@${IP}

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
rsync -a -e 'ssh -i ~/.ssh/scs_chameleon_pass' --progress /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/CMakeLists.txt cc@${IP}:~/pmemcpy
rsync -a -e 'ssh -i ~/.ssh/scs_chameleon_pass' --progress /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/include cc@${IP}:~/pmemcpy
rsync -a -e 'ssh -i ~/.ssh/scs_chameleon_pass' --progress /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/src cc@${IP}:~/pmemcpy
rsync -a -e 'ssh -i ~/.ssh/scs_chameleon_pass' --progress /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/test cc@${IP}:~/pmemcpy
rsync -a -e 'ssh -i ~/.ssh/scs_chameleon_pass' --progress /home/lukemartinlogan/Documents/Projects/PhD/pmemcpy/scripts cc@${IP}:~/pmemcpy

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
sudo chown -R cc /home/cc/tmpfs
sudo umount /home/cc/tmpfs

#ALL TESTS
export CPATH="/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/parallel-netcdf-1.12.2-ejnehvefuraqlpmya24jporxczhqg7j4/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/netcdf-c-4.8.1-xcsyxgya5wdhstixxbthkftb4c3ue24a/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/capnproto-0.8.0-t3gxgugdkn7nwdx5wrvrfpcgcudn6xgm/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/openmpi-4.1.1-appr545p7jtqiam3pygjkseyzdzdlssa/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/adios-1.13.1-5icuhwvnnrmvf3ifytlzgshi7sk5aorn/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/hdf5-1.10.7-zywvux2r43l2uns4qwj3notysg2jsg3s/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/boost-1.77.0-dkjimsg6v3plowbxgj3hy3l35gpqomz6/include:/home/cc/spack/opt/spack/linux-ubuntu20.04-skylake_avx512/gcc-9.3.0/pmdk-1.11.0-ojpodkzcgoiir7bs5m5bbvts7emkhufk/include"
export INCLUDE=$CPATH
bash ~/pmemcpy/scripts/tests.sh
scp -i ~/.ssh/id_rsa_pass cc@${IP}:~/pmemcpy/scripts/log.txt log.txt

#
python3
import pandas as pd
df = pd.read_csv("log.txt", sep=" ")
cols = list(df.columns)
cols.remove("time")
df = df.groupby(cols).mean().reset_index()
df["time2"] = df["time"] + 5
df.to_csv("averages2.csv")