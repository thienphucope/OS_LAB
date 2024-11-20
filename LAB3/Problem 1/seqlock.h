#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock {
    pthread_mutex_t mutex;
    unsigned int sequence;
    int count_readers;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
    pthread_mutex_init(&rw->mutex, NULL);
    rw->sequence = 0;
    rw->count_readers = 0;
}

static inline int pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
    pthread_mutex_lock(&rw->mutex);
    // kiểm tra writer hoặc reader đang hoạt động trong critical section
    if (rw->sequence % 2 == 0 && rw->count_readers == 0 ) {
        rw->sequence++;
        pthread_mutex_unlock(&rw->mutex);
        return 1; 
    }
    pthread_mutex_unlock(&rw->mutex);
    return 0; 
}


static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
    pthread_mutex_lock(&rw->mutex);
    rw->sequence++;
    pthread_mutex_unlock(&rw->mutex);
}

static inline int pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
    pthread_mutex_lock(&rw->mutex);
    // kiểm tra writer đang hoạt động trong critical section, 
    if (rw->sequence % 2 == 0) {
        rw->count_readers++; 
        pthread_mutex_unlock(&rw->mutex);
        return 1; 

    }
    pthread_mutex_unlock(&rw->mutex);

    return 0; 

}

static inline void pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
    pthread_mutex_lock(&rw->mutex);
    rw->count_readers--;
    rw->sequence++; 
    pthread_mutex_unlock(&rw->mutex);
}

