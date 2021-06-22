#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
EXEC_DIR=${SCRIPT_DIR}/../cmake-build-debug/test/performance/e2e
SOURCE_DIR=${SCRIPT_DIR}/../test/performance/e2e/3d
PMEM_DIR=/home/cc/tmpfs

function test_case() {
  suffix=$1
  nprocs=$2
  npx=$3
  npy=$4
  npz=$5
  ndx=$6
  ndy=$7
  ndz=$8
  storage=$9
  serializer=${10}
  use_mmap=${11}
  rm -rf ${PMEM_DIR}/*
  if [ ${suffix} == "bp" ]; then
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${SOURCE_DIR}/3d.xml ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    sudo sync
    sudo sysctl -w vm.drop_caches=3 >> /dev/null
    sudo umount ~/tmpfs
    sudo mount -o dax /dev/pmem0 ~/tmpfs
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  elif [ ${suffix} == "pmemcpy_omp" ]; then
    mkdir ${PMEM_DIR}/test
    ${EXEC_DIR}/write_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz} ${storage} ${serializer} ${use_mmap}
    sudo sync
    sudo sysctl -w vm.drop_caches=3 >> /dev/null
    ${EXEC_DIR}/read_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${storage} ${serializer} ${use_mmap}
  else
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    sudo sync
    sudo sysctl -w vm.drop_caches=3 >> /dev/null
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  fi
  rm -rf ${PMEM_DIR}/*
}


SUFFIXES=("bp")
for SUFFIX in ${SUFFIXES[@]}; do
  test_case ${SUFFIX} 24 4 2 3 384 192 192
done

STORAGE_TYPES=("PMDK_HASHTABLE" "POSIX")
SERIALIZER_TYPES=("NO_SERIALIZER" "CAPNPROTO" "CAPNPROTO_NOCOMPRESS")
for STORAGE_TYPE in ${STORAGE_TYPES[@]}; do
  for SERIALIZER_TYPE in ${SERIALIZER_TYPES[@]}; do
    test_case "pmemcpy_omp" 24 4 2 3 384 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
  done
done
