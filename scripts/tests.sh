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
    echo mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${SOURCE_DIR}/3d.xml ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${SOURCE_DIR}/3d.xml ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    echo mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  elif [ ${suffix} == "pmemcpy_omp" ]; then
    echo ${EXEC_DIR}/write_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz} ${storage} ${serializer} ${use_mmap}
    mkdir ${PMEM_DIR}/test
    ${EXEC_DIR}/write_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz} ${storage} ${serializer} ${use_mmap}
    echo ${EXEC_DIR}/read_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${storage} ${serializer} ${use_mmap}
    ${EXEC_DIR}/read_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${storage} ${serializer} ${use_mmap}
  else
    echo mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    echo mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  fi
  rm -rf ${PMEM_DIR}/*
}

SUFFIXES=()
#SUFFIXES=("bp" "nc4" "nc5")
for SUFFIX in ${SUFFIXES[@]}; do
  test_case ${SUFFIX} 8 2 2 2 1776 192 192
  test_case ${SUFFIX} 16 4 2 2 888 192 192
  test_case ${SUFFIX} 24 4 2 3 592 192 192
  test_case ${SUFFIX} 32 4 4 2 444 192 192
  test_case ${SUFFIX} 48 4 4 3 296 192 192
done

STORAGE_TYPES=("PMDK_HASHTABLE" "POSIX")
#SERIALIZER_TYPES=("NO_SERIALIZER" "CAPNPROTO" "CAPNPROTO_NOCOMPRESS" "MSGPACK" "BOOST" "CEREAL")
SERIALIZER_TYPES=("NO_SERIALIZER" "CAPNPROTO_NOCOMPRESS")

STORAGE_TYPES=("PMDK_HASHTABLE")
STORAGE_TYPES=("POSIX")
SERIALIZER_TYPES=("NO_SERIALIZER" "CAPNPROTO_NOCOMPRESS")

for STORAGE_TYPE in ${STORAGE_TYPES[@]}; do
  for SERIALIZER_TYPE in ${SERIALIZER_TYPES[@]}; do
    test_case "pmemcpy_omp" 8 2 2 2 1776 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
    test_case "pmemcpy_omp" 16 4 2 2 888 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
    test_case "pmemcpy_omp" 24 4 2 3 592 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
    test_case "pmemcpy_omp" 32 4 4 2 444 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
    test_case "pmemcpy_omp" 48 4 4 3 296 192 192 ${STORAGE_TYPE} ${SERIALIZER_TYPE} 1
  done
done
