#ifndef _USG_TASK_H__
#define _USG_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_mutex_t                 USG_MUTEX_ID;
typedef pthread_mutexattr_t             USG_MUTEXATTR_ID;

uint64_t usg_mutex_create(char *semName, USG_MUTEX_ID *semId);
uint64_t usg_mutex_delete(USG_MUTEX_ID *semId);
uint64_t usg_mutex_lock(USG_MUTEX_ID *semId);
uint64_t usg_mutex_unlock(USG_MUTEX_ID *semId);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef  _USG_TASK_H__ */
