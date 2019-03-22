#ifndef _USG_UTIL_H__
#define _USG_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void util_sort_swap1(uint64_t *left, uint64_t *right)
{
    uint64_t tmp;

    if (*left > *right) {
        tmp = *left;
        *left = *right;
        *right = tmp;
    }
}

static inline void util_sort_swap2(uint64_t *left, uint64_t *right)
{
    if (*left > *right) {
        *right = *right + *left;
        *left  = *right - *left;
        *right = *right - *left;
    }
}

static inline void util_sort_swap3(uint64_t *left, uint64_t *right)
{
    if (*left > *right) {
        *right ^= *left;
        *left  ^= *right;
        *right ^= *left;
    }
}

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef  _USG_UTIL_H__ */

