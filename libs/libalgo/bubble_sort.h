#ifndef _BUBBLE_SORT_H__
#define _BUBBLE_SORT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*bs_u64_cb_t)(uint64_t *left, uint64_t *right);

int bubble_sort_u64(uint64_t *arry, uint64_t arrylen, bs_u64_cb_t bsct);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef  _BUBBLE_SORT_H__ */

