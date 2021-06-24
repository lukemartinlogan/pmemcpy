//
// Created by lukemartinlogan on 6/18/21.
//

#ifndef PM_LOGGER_H
#define PM_LOGGER_H

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void log_end1(char *io, char *program, int nprocs, double start_time, double end_time, size_t *readsize, const char *serializer, const char *storage) {
    size_t size_per_proc = 10*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
    size_t agg_size = size_per_proc*nprocs;
    printf("%s %s %d %lu %lu %lu %lu %lu %lf %s %s\n",
           io, program, nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time,
           serializer, storage);

    FILE *out = fopen("log.txt", "a+");
    fprintf(out, "%s %s %d %lu %lu %lu %lu %lu %lf %s %s\n",
            io, program, nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time,
            serializer, storage);
    fclose(out);
}

void log_end2(char *io, char *program, int nprocs, double start_time, double end_time, MPI_Offset *readsize, const char *serializer, const char *storage) {
    size_t size_per_proc = 10*sizeof(double)*readsize[0]*readsize[1]*readsize[2];
    size_t agg_size = size_per_proc*nprocs;
    printf("%s %s %d %lld %lld %lld %lu %lu %lf %s %s\n",
           io, program, nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time,
           serializer, storage);

    FILE *out = fopen("log.txt", "a+");
    fprintf(out, "%s %s %d %lld %lld %lld %lu %lu %lf %s %s\n",
            io, program, nprocs, readsize[0], readsize[1], readsize[2], size_per_proc, agg_size, end_time - start_time,
            serializer, storage);
    fclose(out);
}

#endif //PM_LOGGER_H
