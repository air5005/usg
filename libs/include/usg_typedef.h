#ifndef _USG_TYPEDEF_H__
#define _USG_TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
define int8_t uint8_t int16_t uint16_t int32_t uint32_t int64_t uint64_t
*/
#include <stdint.h>

#define G_SUCCESS               ((uint64_t)0)
#define G_FAILURE               ((uint64_t)(-1))

#define G_TRUE                  (1)
#define G_FALSE                 (0)

#ifndef max
#define max(a,b)                (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)                (((a) < (b)) ? (a) : (b))
#endif

#ifndef __stringify
#define __stringify_1(x) #x
#define __stringify(x)	__stringify_1(x)
#endif

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#ifndef __al_unused
#define __al_unused __attribute__((unused))
#endif

#ifndef __mb_unused
#define __mb_unused __attribute__((unused))
#endif

#ifndef __aligned
#define __aligned(x) __attribute__((aligned(x)))
#endif

/**
 * Force a function to be inlined
 */
#define __al_inline inline __attribute__((always_inline))

/**
 * Force a function to be noinlined
 */
#define __no_inline  __attribute__((noinline))

#ifndef likely
#define likely(x)	__builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)	__builtin_expect(!!(x), 0)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef  _USG_TYPEDEF_H__ */
