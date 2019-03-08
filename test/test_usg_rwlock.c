#include "usg_common.h"

struct test_rwlock_cb {
    uint64_t id;
};
struct test_rwlock_cb test_cb[128];

static usg_rwlock_t sl;
static usg_rwlock_t sl_tab[128];
static pthread_barrier_t start_barrier;
static pthread_barrier_t end_barrier;

static void *
test_rwlock_per_core(void *arg)
{
    struct test_rwlock_cb *cb = (struct test_rwlock_cb *)arg;

	printf("enter core:%ld\n", cb->id);

    pthread_barrier_wait(&start_barrier);

	usg_rwlock_write_lock(&sl);
	printf("Global write lock taken on core %ld\n", cb->id);
	usg_rwlock_write_unlock(&sl);

	usg_rwlock_write_lock(&sl_tab[cb->id]);
	printf("Hello from core %ld !\n", cb->id);
	usg_rwlock_write_unlock(&sl_tab[cb->id]);

	usg_rwlock_read_lock(&sl);
	printf("Global read lock taken on core %ld\n", cb->id);
	usleep(100*1000);
	printf("Release global read lock on core %ld\n", cb->id);
	usg_rwlock_read_unlock(&sl);

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

	if (argc >= 2) taskNum =  atoi(argv[1]);

    max_cpu = sysconf(_SC_NPROCESSORS_CONF);
    if ((taskNum > max_cpu) || (taskNum == 0))
        taskNum = max_cpu;

    memset(test_cb, 0, sizeof(test_cb));

    pthread_barrier_init(&start_barrier, NULL, taskNum+1);
    pthread_barrier_init(&end_barrier, NULL, taskNum+1);

	usg_rwlock_init(&sl);
	for (index = 0; index < taskNum; index++)
		usg_rwlock_init(&sl_tab[index]);

	usg_rwlock_write_lock(&sl);

    for (index = 0; index < taskNum; index ++) {
        test_cb[index].id = index;
		usg_rwlock_write_lock(&sl_tab[index]);
        ret = pthread_create(&thread_id, NULL, test_rwlock_per_core,
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

	usg_rwlock_write_unlock(&sl);

    pthread_barrier_wait(&start_barrier);

	for (index = 0; index < taskNum; index ++) {
		usg_rwlock_write_unlock(&sl_tab[index]);
    	usleep(100*1000);
	}

	usg_rwlock_write_lock(&sl);
	/* this message should be the last message of test */
	printf("this message should be the last message of test\n");
	usg_rwlock_write_unlock(&sl);

    pthread_barrier_wait(&end_barrier);

    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);
	printf("rwlock end of test.\n");
	return 0;
}

