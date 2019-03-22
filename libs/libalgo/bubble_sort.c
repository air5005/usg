#include "usg_common.h"
#include "bubble_sort.h"

void bubble_sort_u64_swap(uint64_t *left, uint64_t *right)
{
#if 1
    uint64_t tmp;

    if (*left > *right) {
        tmp = *left;
        *left = *right;
        *right = tmp;
    }
#else
    if (*left > *right) {
        *right = *right + *left;
        *left  = *right - *left;
        *right = *right - *left;
    }
#endif
}

int bubble_sort_u64(uint64_t *arry, uint64_t arrylen, bs_u64_cb_t bsct)
{
    uint64_t i, j;

    if ((arry == NULL) || (arrylen == 0)) return -1;

    for (i = 0; i < (arrylen - 1); i++) {
        for (j = 1; j < (arrylen - i); j++) {
            if (bsct)
                bsct(&arry[j-1], &arry[j]);
            else
                bubble_sort_u64_swap(&arry[j-1], &arry[j]);
        }
    }

    return 0;
}

