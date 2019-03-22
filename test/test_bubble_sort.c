#include "usg_common.h"

int main(int argc, char **argv)
{
    uint64_t *arry;
    uint64_t  arrylen;
    uint64_t  index;

	if (argc < 2) {
        printf("Incorrect input parameters\r\n");
        return -1;
	}

    arrylen = argc - 1;
    arry = malloc(sizeof(uint64_t) * arrylen);
    if (arry == NULL) {
        printf("malloc fail\r\n");
        return -1;
    }

    for (index = 0; index < arrylen; index ++) {
        arry[index] = (uint64_t)atoi(argv[index+1]);
    }

    for (index = 0; index < arrylen; index ++) {
        printf("arry[%ld]:%ld \r\n", index, arry[index]);
    }

    bubble_sort_u64(arry, arrylen, NULL);

    for (index = 0; index < arrylen; index ++) {
        printf("arry[%ld]:%ld \r\n", index, arry[index]);
    }

	return 0;
}

