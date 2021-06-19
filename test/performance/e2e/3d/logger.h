//
// Created by lukemartinlogan on 6/18/21.
//

#ifndef PM_LOGGER_H
#define PM_LOGGER_H

#include <stdlib.h>

void log_start(int npx, int npy, int npz) {

}

void log_end(char *suffix, int nprocs, double start_time, double end_time, uint64_t *readsize) {
    size_t size_per_proc = 3*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
    size_t agg_size = size_per_proc*nprocs;
    printf("read bp %lu %lu %lu %lu %lu %lu %lf none none\n", nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time);

    FILE *out = fopen("log.txt");
    fprintf(out, )
}

void log_end(char *suffix, int nprocs, double start_time, double end_time, MPI_Offset *readsize) {
    size_t size_per_proc = 3*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
    size_t agg_size = size_per_proc*nprocs;
    printf("read bp %lu %lu %lu %lu %lu %lu %lf none none\n", nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time);

    FILE *out = fopen("log.txt");
    fprintf(out, )
}

#endif //PM_LOGGER_H
