#include "usg_common.h"
#include "bubble_sort.h"

int bubble_sort_u64(uint64_t *arry, uint64_t arrylen, bs_u64_cb_t bsct)
{
    uint64_t i, j;

    if ((arry == NULL) || (arrylen == 0)) return -1;

    for (i = 0; i < (arrylen - 1); i++) {
        for (j = 1; j < (arrylen - i); j++) {
            if (bsct)
                bsct(&arry[j-1], &arry[j]);
            else
                util_sort_swap3(&arry[j-1], &arry[j]);
        }
    }

    return 0;
}

