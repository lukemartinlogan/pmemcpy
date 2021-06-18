#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
EXEC_DIR=${SCRIPT_DIR}/../cmake-build-debug/test/performance/e2e-1
SOURCE_DIR=${SCRIPT_DIR}/../test/performance/e2e-1/3d
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
  rm -rf ${PMEM_DIR}/*
  if [ ${suffix} == "bp" ]; then
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${SOURCE_DIR}/3d.xml ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  elif [ ${suffix} == "pmemcpy_omp" ]; then
    mkdir ${PMEM_DIR}/test
    ${EXEC_DIR}/write_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz} ${storage} ${serializer}
    ${EXEC_DIR}/read_3d_pmemcpy_omp ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${storage} ${serializer}
  else
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/write_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz} ${ndx} ${ndy} ${ndz}
    mpirun -n ${nprocs} --use-hwthread-cpus ${EXEC_DIR}/read_3d_${suffix} ${PMEM_DIR}/test ${npx} ${npy} ${npz}
  fi
  rm -rf ${PMEM_DIR}/*
}

STORAGE=("POSIX" "PMDK_HASHTABLE")
SERIALIZERS=("CAPNPROTO" "BOOST" "MSGPACK" "CEREAL")

SUFFIXES=("bp" "nc4" "nc5")
#for SUFFIX in ${SUFFIXES[@]}; do
  #test_case ${SUFFIX} 8 2 2 2 192 192 192
  #test_case ${SUFFIX} 16 4 2 2 192 192 192
  #test_case ${SUFFIX} 24 4 2 3 192 192 192
  #test_case ${SUFFIX} 32 4 4 2 192 192 192
  #test_case ${SUFFIX} 40 2 4 5 192 192 192
  #test_case ${SUFFIX} 48 4 4 3 192 192 192
#done

STORAGE_TYPES=("PMDK_HASHTABLE" "POSIX")
for STORAGE_TYPE in ${STORAGE_TYPES[@]}; do
  #test_case "pmemcpy_omp" 8 2 2 2 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
  #test_case "pmemcpy_omp" 16 4 2 2 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
  #test_case "pmemcpy_omp" 24 4 2 3 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
  #test_case "pmemcpy_omp" 32 4 4 2 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
  #test_case "pmemcpy_omp" 40 2 4 5 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
  test_case "pmemcpy_omp" 48 4 4 3 192 192 192 ${STORAGE_TYPE} "CAPNPROTO"
done

SERIALIZER_TYPES=("CAPNPROTO" "MSGPACK" "BOOST" "CEREAL")
#for SERIALIZER_TYPE in ${SERIALIZER_TYPES[@]}; do
  #test_case "pmemcpy_omp" 8 2 2 2 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
  #test_case "pmemcpy_omp" 16 4 2 2 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
  #test_case "pmemcpy_omp" 24 4 2 3 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
  #test_case "pmemcpy_omp" 32 4 4 2 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
  #test_case "pmemcpy_omp" 40 2 4 5 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
  #test_case "pmemcpy_omp" 48 4 4 3 192 192 192 "PMDK_HASHTABLE" ${SERIALIZER_TYPE}
#done
