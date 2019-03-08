#include "usg_common.h"

#define TEST_READ_ONCE(x) __atomic_load_n((typeof(x) *)&(x), __ATOMIC_RELAXED)
#define TEST_WRITE_ONCE(x, v) __atomic_store_n((typeof(x) *)&(x), (v), __ATOMIC_RELAXED)

struct test_rwlock_cb {
    uint64_t id;
    uint64_t count;
};
struct test_rwlock_cb test_cb[128];

static pthread_barrier_t start_barrier;
static pthread_barrier_t end_barrier;

volatile uint64_t total_counter = 0;
usg_atomic64_t atomic_counter;

static __inline__ void count_init(void)
{
	TEST_WRITE_ONCE(total_counter, 0);
}

static __inline__ void inc_count1(void)
{
    total_counter++;
    usg_mb();
}

static __inline__ uint64_t read_count1(void)
{
	return total_counter;
}

static __inline__ void inc_count2(void)
{
	TEST_WRITE_ONCE(total_counter, TEST_READ_ONCE(total_counter) + 1);
}

static __inline__ uint64_t read_count2(void)
{
	return TEST_READ_ONCE(total_counter);
}

static __inline__ void inc_count3(void)
{
	usg_atomic64_inc(&atomic_counter);
}

static __inline__ uint64_t read_count3(void)
{
	return (uint64_t)usg_atomic64_read(&atomic_counter);
}

static void *
test_count_per_core1(void *arg)
{
    struct test_rwlock_cb *cb = (struct test_rwlock_cb *)arg;
    uint64_t index;

    pthread_barrier_wait(&start_barrier);
    for (index = 0; index < cb->count; index++) {
        inc_count1();
    }
    pthread_barrier_wait(&end_barrier);
	return 0;
}

static void *
test_count_per_core2(void *arg)
{
    struct test_rwlock_cb *cb = (struct test_rwlock_cb *)arg;
    uint64_t index;

    pthread_barrier_wait(&start_barrier);
    for (index = 0; index < cb->count; index++) {
        inc_count2();
    }
    pthread_barrier_wait(&end_barrier);
	return 0;
}

static void *
test_count_per_core3(void *arg)
{
    struct test_rwlock_cb *cb = (struct test_rwlock_cb *)arg;
    uint64_t index;

    pthread_barrier_wait(&start_barrier);
    for (index = 0; index < cb->count; index++) {
        inc_count3();
    }
    pthread_barrier_wait(&end_barrier);
	return 0;
}

int main(int argc, char **argv)
{
    int taskNum = 0;
    int ret;
    int index;
    pthread_t thread_id;
    cpu_set_t cpuset;
    uint8_t  max_cpu;
    uint64_t count = 1000000;

	if (argc >= 2) count   =  atoi(argv[1]);
	if (argc >= 3) taskNum =  atoi(argv[2]);

    max_cpu = sysconf(_SC_NPROCESSORS_CONF);
    if ((taskNum > max_cpu) || (taskNum == 0))
        taskNum = max_cpu;

    memset(test_cb, 0, sizeof(test_cb));

    pthread_barrier_init(&start_barrier, NULL, taskNum+1);
    pthread_barrier_init(&end_barrier, NULL, taskNum+1);

    for (index = 0; index < taskNum; index ++) {
        test_cb[index].id = index;
        test_cb[index].count = count;
        ret = pthread_create(&thread_id, NULL, test_count_per_core1,
                                (void *)&test_cb[index]);
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

    count_init();
    pthread_barrier_wait(&start_barrier);
    pthread_barrier_wait(&end_barrier);
    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);

    printf("read_count1:%ld \n", read_count1());
    /**************************************************************************/

    memset(test_cb, 0, sizeof(test_cb));

    pthread_barrier_init(&start_barrier, NULL, taskNum+1);
    pthread_barrier_init(&end_barrier, NULL, taskNum+1);

    for (index = 0; index < taskNum; index ++) {
        test_cb[index].id = index;
        test_cb[index].count = count;
        ret = pthread_create(&thread_id, NULL, test_count_per_core2,
                                (void *)&test_cb[index]);
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

    count_init();
    pthread_barrier_wait(&start_barrier);
    pthread_barrier_wait(&end_barrier);
    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);

    printf("read_count2:%ld \n", read_count2());
    /**************************************************************************/

    memset(test_cb, 0, sizeof(test_cb));

    pthread_barrier_init(&start_barrier, NULL, taskNum+1);
    pthread_barrier_init(&end_barrier, NULL, taskNum+1);

    for (index = 0; index < taskNum; index ++) {
        test_cb[index].id = index;
        test_cb[index].count = count;
        ret = pthread_create(&thread_id, NULL, test_count_per_core3,
                                (void *)&test_cb[index]);
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

    usg_atomic64_init(&atomic_counter);
    pthread_barrier_wait(&start_barrier);
    pthread_barrier_wait(&end_barrier);
    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);

    printf("read_count3:%ld \n", read_count3());
    /**************************************************************************/

	printf("end of test.\n");
	return 0;
}

