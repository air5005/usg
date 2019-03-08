#include "usg_common.h"

static usg_spinlock_t sl;
static usg_spinlock_t sl_tab[128];

static pthread_barrier_t start_barrier;
static pthread_barrier_t end_barrier;

struct test_splock_cb {
    uint64_t id;
};

struct test_splock_cb test_cb[128];

static void *
test_spinlock_per_core(void *arg)
{
    struct test_splock_cb *cb = (struct test_splock_cb *)arg;

	printf("enter core:%ld\n", cb->id);

    pthread_barrier_wait(&start_barrier);

	usg_spinlock_lock(&sl);
	printf("core %ld stausgd \n", cb->id);
	usg_spinlock_unlock(&sl);

	printf("core:%ld, sl is lock: %u\n", cb->id,
                        usg_spinlock_is_locked(&sl));

	printf("Hello from core %ld !\n", cb->id);
	usg_spinlock_unlock(&sl_tab[cb->id]);

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
	usg_spinlock_init(&sl);
	for (index = 0; index < taskNum; index++)
		usg_spinlock_init(&sl_tab[index]);

	usg_spinlock_lock(&sl);

    for (index = 0; index < taskNum; index ++) {
        test_cb[index].id = index;
        usg_spinlock_lock(&sl_tab[index]);
        ret = pthread_create(&thread_id, NULL, test_spinlock_per_core,
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

    pthread_barrier_wait(&start_barrier);

	printf("The creation task is completed and the subtask can be stausgd.\n");
	usg_spinlock_unlock(&sl);

    for (index = 0; index < taskNum; index ++) {
        while (usg_spinlock_is_locked(&sl_tab[index]))
            usleep(1000);
    }

    pthread_barrier_wait(&end_barrier);

    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);

	printf("spinlock end of test.\n");
	return 0;
}

