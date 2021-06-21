//
// Created by lukemartinlogan on 6/20/21.
//

#ifndef PM_PMEMULATOR_H
#define PM_PMEMULATOR_H

#include <linux/uio.h>
#include <linux/delay.h>

#define DRAM_BW 68
#define DRAM_LATENCY 100
#define PMEM_READ_BW 30
#define PMEM_WRITE_BW 8
#define PMEM_READ_LATENCY 300
#define PMEM_WRITE_LATENCY 125
inline void nsleep(long delay) {
    ndelay(delay);
}
#define READ_PENALTY(iov) (long)((PMEM_READ_LATENCY - DRAM_LATENCY) + (iov_iter_count(iov)/PMEM_READ_BW - iov_iter_count(iov)/DRAM_BW))
#define WRITE_PENALTY(iov) (long)((PMEM_WRITE_LATENCY - DRAM_LATENCY) + (iov_iter_count(iov)/PMEM_WRITE_BW - iov_iter_count(iov)/DRAM_BW))

#ifdef NO_PMEMULATION
#define ADD_READ_PENALTY(iov)
#define ADD_WRITE_PENALTY(iov)
#else
#define ADD_READ_PENALTY(iov) nsleep(READ_PENALTY(iov))
#define ADD_WRITE_PENALTY(iov) nsleep(WRITE_PENALTY(iov))
#endif

#ifdef DEBUG
#define DEBUG_READ(iov) printk("pmemulator:read: sleeps for %ld ns (%lu MB)\n", READ_PENALTY(iov), iov_iter_count(iov)/(1<<20));
#define DEBUG_WRITE(iov) printk("pmemulator:write: sleeps for %ld ns (%lu MB)\n", WRITE_PENALTY(iov), iov_iter_count(iov)/(1<<20));
#else
#define DEBUG_READ(iov)
#define DEBUG_WRITE(iov)
#endif

#endif //PM_PMEMULATOR_H
