#include "usg_common.h"
#include "usg_lock.h"

uint64_t usg_mutex_create(
                __mb_unused char *semName,
                USG_MUTEX_ID *semId,
                __mb_unused const char *fileName,
                __mb_unused uint64_t line)
{
    int ret;
    USG_MUTEXATTR_ID  semAttr;

    if (unlikely(NULL == semId)) {
        return G_FAILURE;
    }

    ret = pthread_mutexattr_init(&semAttr);
    if (unlikely(ret != 0)) {
        return G_FAILURE;
    }

    pthread_mutexattr_settype(&semAttr, PTHREAD_MUTEX_RECURSIVE);
    ret = pthread_mutex_init(semId, &semAttr);
    if (unlikely(ret != 0)) {
        return G_FAILURE;
    }

    return G_SUCCESS;
}

uint64_t usg_mutex_delete(USG_MUTEX_ID *semId,
                __mb_unused const char *fileName,
                __mb_unused uint64_t line)
{
    if (unlikely(NULL == semId)) {
        return G_FAILURE;
    }

    pthread_mutex_destroy(semId);
    return G_SUCCESS;
}

uint64_t usg_mutex_lock(USG_MUTEX_ID *semId,
                __mb_unused const char *fileName,
                __mb_unused uint64_t line)
{
    if (unlikely(NULL == semId)) {
        return G_FAILURE;
    }

    pthread_mutex_lock(semId);
    return G_SUCCESS;
}

uint64_t usg_mutex_unlock(USG_MUTEX_ID *semId,
                __mb_unused const char *fileName,
                __mb_unused uint64_t line)
{
    if (unlikely(NULL == semId)) {
        return G_FAILURE;
    }

    pthread_mutex_unlock(semId);
    return G_SUCCESS;
}

