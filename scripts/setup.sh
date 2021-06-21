make build-pmemulator
make clean-klog
make insert-pmemulator
sudo mount -t pmemulator-ext4 /dev/sdb ~/tmpfs
sudo mount -t pmemulator-ext4 -o dax /dev/sdb ~/tmpfs
sudo umount ~/tmpfs
make rm-pmemulator

ssh cc@129.114.109.179

#COPY pmemcpy to Chameleon
ROOT_DIR=../
tar -czf pmemcpy.tar.gz ${ROOT_DIR}/include ${ROOT_DIR}/src ${ROOT_DIR}/test ${ROOT_DIR}/scripts ${ROOT_DIR}/CMakeLists.txt
scp pmemcpy.tar.gz cc@129.114.109.179:~/

#DECOMPRESS pmemcpy on Chameleon
tar -xzf ${HOME}/pmemcpy.tar.gz -C ${HOME}/pmemcpy

#BUILD pmemcpy
cd ${HOME}/pmemcpy
mkdir cmake-build-debug
cd cmake-build-debug
cmake ../
make -j8
make build-pmemulator
make clean-klog
make insert-pmemulator
make rm-pmemulator

#MAKE filesystem
mkdir ~/tmpfs
sudo mkfs.ext4 /dev/pmem0
sudo mount -o dax /dev/pmem0 ~/tmpfs
sudo mount -t pmemulator-ext4 -o dax /dev/pmem0 ~/tmpfs
sudo chown -R cc ~/tmpfs
sudo umount ~/tmpfs


#TESTS
bash ~/pmemcpy/scripts/tests.sh