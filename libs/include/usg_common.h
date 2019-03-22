#ifndef _USG_COMMON_H__
#define _USG_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <emmintrin.h>

#include "usg_typedef.h"
#include "usg_atomic.h"
#include "usg_spinlock.h"
#include "usg_rwlock.h"
#include "usg_cycles.h"
#include "usg_lock.h"
#include "usg_ring.h"

#include "bubble_sort.h"

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef  _USG_COMMON_H__ */

