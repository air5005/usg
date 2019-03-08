#include "usg_common.h"

#define NUM_ATOMIC_TYPES 3

#define N 10000

static usg_atomic16_t a16;
static usg_atomic32_t a32;
static usg_atomic64_t a64;
static usg_atomic64_t count;
static usg_atomic32_t synchro;
static usg_atomic32_t tiff;

static void *
test_atomic_usual(__mb_unused void *arg)
{
	unsigned i;

	while (usg_atomic32_read(&synchro) == 0)
		;

    usg_atomic32_inc(&tiff);

	for (i = 0; i < N; i++)
		usg_atomic16_inc(&a16);
	for (i = 0; i < N; i++)
		usg_atomic16_dec(&a16);
	for (i = 0; i < (N / 5); i++)
		usg_atomic16_add(&a16, 5);
	for (i = 0; i < (N / 5); i++)
		usg_atomic16_sub(&a16, 5);

	for (i = 0; i < N; i++)
		usg_atomic32_inc(&a32);
	for (i = 0; i < N; i++)
		usg_atomic32_dec(&a32);
	for (i = 0; i < (N / 5); i++)
		usg_atomic32_add(&a32, 5);
	for (i = 0; i < (N / 5); i++)
		usg_atomic32_sub(&a32, 5);

	for (i = 0; i < N; i++)
		usg_atomic64_inc(&a64);
	for (i = 0; i < N; i++)
		usg_atomic64_dec(&a64);
	for (i = 0; i < (N / 5); i++)
		usg_atomic64_add(&a64, 5);
	for (i = 0; i < (N / 5); i++)
		usg_atomic64_sub(&a64, 5);

	return 0;
}

static void *
test_atomic_tas(__mb_unused void *arg)
{
	while (usg_atomic32_read(&synchro) == 0)
		;

    usg_atomic32_inc(&tiff);

	if (usg_atomic16_test_and_set(&a16))
		usg_atomic64_inc(&count);
	if (usg_atomic32_test_and_set(&a32))
		usg_atomic64_inc(&count);
	if (usg_atomic64_test_and_set(&a64))
		usg_atomic64_inc(&count);

	return 0;
}

static void *
test_atomic_addsub_and_return(__mb_unused void *arg)
{
	uint32_t tmp16;
	uint32_t tmp32;
	uint64_t tmp64;
	unsigned i;

	while (usg_atomic32_read(&synchro) == 0)
		;

    usg_atomic32_inc(&tiff);

	for (i = 0; i < N; i++) {
		tmp16 = usg_atomic16_add_return(&a16, 1);
		usg_atomic64_add(&count, tmp16);

		tmp16 = usg_atomic16_sub_return(&a16, 1);
		usg_atomic64_sub(&count, tmp16+1);

		tmp32 = usg_atomic32_add_return(&a32, 1);
		usg_atomic64_add(&count, tmp32);

		tmp32 = usg_atomic32_sub_return(&a32, 1);
		usg_atomic64_sub(&count, tmp32+1);

		tmp64 = usg_atomic64_add_return(&a64, 1);
		usg_atomic64_add(&count, tmp64);

		tmp64 = usg_atomic64_sub_return(&a64, 1);
		usg_atomic64_sub(&count, tmp64+1);
	}

	return 0;
}

/*
 * usg_atomic32_inc_and_test() would increase a 32 bits counter by one and then
 * test if that counter is equal to 0. It would return true if the counter is 0
 * and false if the counter is not 0. usg_atomic64_inc_and_test() could do the
 * same thing but for a 64 bits counter.
 * Here checks that if the 32/64 bits counter is equal to 0 after being atomically
 * increased by one. If it is, increase the variable of "count" by one which would
 * be checked as the result later.
 *
 */
static void *
test_atomic_inc_and_test(__mb_unused void *arg)
{
	while (usg_atomic32_read(&synchro) == 0)
		;

    usg_atomic32_inc(&tiff);

	if (usg_atomic16_inc_and_test(&a16)) {
		usg_atomic64_inc(&count);
	}
	if (usg_atomic32_inc_and_test(&a32)) {
		usg_atomic64_inc(&count);
	}
	if (usg_atomic64_inc_and_test(&a64)) {
		usg_atomic64_inc(&count);
	}

	return 0;
}

/*
 * usg_atomicXX_dec_and_test() should decrease a 32 bits counter by one and then
 * test if that counter is equal to 0. It should return true if the counter is 0
 * and false if the counter is not 0.
 * This test checks if the counter is equal to 0 after being atomically
 * decreased by one. If it is, increase the value of "count" by one which is to
 * be checked as the result later.
 */
static void *
test_atomic_dec_and_test(__mb_unused void *arg)
{
	while (usg_atomic32_read(&synchro) == 0)
		;

    usg_atomic32_inc(&tiff);

	if (usg_atomic16_dec_and_test(&a16))
		usg_atomic64_inc(&count);

	if (usg_atomic32_dec_and_test(&a32))
		usg_atomic64_inc(&count);

	if (usg_atomic64_dec_and_test(&a64))
		usg_atomic64_inc(&count);

	return 0;
}

int main(int argc, char **argv)
{
    int taskNum = 4;
    int ret;
    int index;
    pthread_t thread_id;
    cpu_set_t cpuset;

	if (argc >= 2) taskNum =  atoi(argv[1]);

	usg_atomic16_init(&a16);
	usg_atomic32_init(&a32);
	usg_atomic64_init(&a64);
	usg_atomic64_init(&count);
	usg_atomic32_init(&synchro);
	usg_atomic32_init(&tiff);

	usg_atomic16_set(&a16, 1UL << 10);
	usg_atomic32_set(&a32, 1UL << 10);
	usg_atomic64_set(&a64, 1ULL << 33);

	printf("usual inc/dec/add/sub functions\n");

    for (index = 0; index < taskNum; index ++) {
        ret = pthread_create(&thread_id, NULL, test_atomic_usual, NULL);
        if (ret != 0) {
            printf("pthread_create fail\n");
            return -1;
        }
        CPU_ZERO(&cpuset);
        CPU_SET(index, &cpuset);
        if (pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpuset) != 0)
        {
            printf("pthread_setaffinity_np fail \n");
            return -1;
        }
    }

	usg_atomic32_set(&synchro, 1);
	while (usg_atomic32_read(&tiff) != taskNum)
		usleep(100*1000);
	usg_atomic32_clear(&tiff);
	usg_atomic32_set(&synchro, 0);

	if (usg_atomic16_read(&a16) != 1UL << 10) {
		printf("Atomic16 usual functions failed\n");
		return -1;
	}

	if (usg_atomic32_read(&a32) != 1UL << 10) {
		printf("Atomic32 usual functions failed\n");
		return -1;
	}

	if (usg_atomic64_read(&a64) != 1ULL << 33) {
		printf("Atomic64 usual functions failed\n");
		return -1;
	}

	printf("test and set\n");

	usg_atomic64_set(&a64, 0);
	usg_atomic32_set(&a32, 0);
	usg_atomic16_set(&a16, 0);
	usg_atomic64_set(&count, 0);

    for (index = 0; index < taskNum; index ++) {
        ret = pthread_create(&thread_id, NULL, test_atomic_tas, NULL);
        if (ret != 0) {
            printf("pthread_create fail\n");
            return -1;
        }
        CPU_ZERO(&cpuset);
        CPU_SET(index, &cpuset);
        if (pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpuset) != 0)
        {
            printf("pthread_setaffinity_np fail \n");
            return -1;
        }
    }

	usg_atomic32_set(&synchro, 1);
	while (usg_atomic32_read(&tiff) != taskNum)
		usleep(100*1000);
	usg_atomic32_clear(&tiff);
	usg_atomic32_set(&synchro, 0);

	if (usg_atomic64_read(&count) != NUM_ATOMIC_TYPES) {
		printf("Atomic test and set failed\n");
		return -1;
	}

	printf("add/sub and return\n");

	usg_atomic64_set(&a64, 0);
	usg_atomic32_set(&a32, 0);
	usg_atomic16_set(&a16, 0);
	usg_atomic64_set(&count, 0);

    for (index = 0; index < taskNum; index ++) {
        ret = pthread_create(&thread_id, NULL,
            test_atomic_addsub_and_return, NULL);
        if (ret != 0) {
            printf("pthread_create fail\n");
            return -1;
        }
        CPU_ZERO(&cpuset);
        CPU_SET(index, &cpuset);
        if (pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpuset) != 0)
        {
            printf("pthread_setaffinity_np fail \n");
            return -1;
        }
    }

	usg_atomic32_set(&synchro, 1);
	while (usg_atomic32_read(&tiff) != taskNum)
		usleep(100*1000);
	usg_atomic32_clear(&tiff);
	usg_atomic32_set(&synchro, 0);

	if (usg_atomic64_read(&count) != 0) {
		printf("Atomic add/sub+return failed\n");
		return -1;
	}

	/*
	 * Set a64, a32 and a16 with the same value of minus "number of slave
	 * lcores", launch all slave lcores to atomically increase by one and
	 * test them respectively.
	 * Each lcore should have only one chance to increase a64 by one and
	 * then check if it is equal to 0, but there should be only one lcore
	 * that finds that it is 0. It is similar for a32 and a16.
	 * Then a variable of "count", initialized to zero, is increased by
	 * one if a64, a32 or a16 is 0 after being increased and tested
	 * atomically.
	 * We can check if "count" is finally equal to 3 to see if all slave
	 * lcores performed "atomic inc and test" right.
	 */
	printf("inc and test\n");

	usg_atomic64_clear(&a64);
	usg_atomic32_clear(&a32);
	usg_atomic16_clear(&a16);
	usg_atomic32_clear(&synchro);
	usg_atomic64_clear(&count);

	usg_atomic64_set(&a64, (int64_t)(1 - (int64_t)taskNum));
	usg_atomic32_set(&a32, (int32_t)(1 - (int32_t)taskNum));
	usg_atomic16_set(&a16, (int16_t)(1 - (int16_t)taskNum));

    for (index = 0; index < taskNum; index ++) {
        ret = pthread_create(&thread_id, NULL,
            test_atomic_inc_and_test, NULL);
        if (ret != 0) {
            printf("pthread_create fail\n");
            return -1;
        }
        CPU_ZERO(&cpuset);
        CPU_SET(index, &cpuset);
        if (pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpuset) != 0)
        {
            printf("pthread_setaffinity_np fail \n");
            return -1;
        }
    }

	usg_atomic32_set(&synchro, 1);
	while (usg_atomic32_read(&tiff) != taskNum)
		usleep(100*1000);
	usg_atomic32_clear(&tiff);
	usg_atomic32_clear(&synchro);

	if (usg_atomic64_read(&count) != NUM_ATOMIC_TYPES) {
		printf("Atomic inc and test failed %d\n", (int)count.cnt);
		return -1;
	}

	/*
	 * Same as above, but this time we set the values to "number of slave
	 * lcores", and decrement instead of increment.
	 */
	printf("dec and test\n");

	usg_atomic32_clear(&synchro);
	usg_atomic64_clear(&count);

	usg_atomic64_set(&a64, (int64_t)(taskNum - 1));
	usg_atomic32_set(&a32, (int32_t)(taskNum - 1));
	usg_atomic16_set(&a16, (int16_t)(taskNum - 1));

    for (index = 0; index < taskNum; index ++) {
        ret = pthread_create(&thread_id, NULL,
            test_atomic_dec_and_test, NULL);
        if (ret != 0) {
            printf("pthread_create fail\n");
            return -1;
        }
        CPU_ZERO(&cpuset);
        CPU_SET(index, &cpuset);
        if (pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpuset) != 0)
        {
            printf("pthread_setaffinity_np fail \n");
            return -1;
        }
    }

	usg_atomic32_set(&synchro, 1);
	while (usg_atomic32_read(&tiff) != taskNum)
		usleep(100*1000);
	usg_atomic32_clear(&tiff);
	usg_atomic32_clear(&synchro);

	if (usg_atomic64_read(&count) != NUM_ATOMIC_TYPES) {
		printf("Atomic dec and test failed\n");
		return -1;
	}

	return 0;
}

