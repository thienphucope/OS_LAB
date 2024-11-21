#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex để đảm bảo an toàn luồng
sem_t empty; // Semaphore để theo dõi các slot trống trong bộ đệm
sem_t full;  // Semaphore để theo dõi các slot đã đầy trong bộ đệm
int count;

struct _args {
    unsigned int interval;
};

void flushlog();

void *wrlog(void *data) {

    sem_wait(&empty); // Chờ cho đến khi có một slot trống trong bộ đệm

    pthread_mutex_lock(&mutex); 
    char str[MAX_LOG_LENGTH];
    int id = *(int *)data;
    usleep(20);
    sprintf(str, "%d", id);
    strcpy(logbuf[count], str);
    count++; 

    pthread_mutex_unlock(&mutex); 

    if ( count == MAX_BUFFER_SLOT ) sem_post(&full);

    return NULL;
}

void flushlog() {
    int i;
    char nullval[MAX_LOG_LENGTH];

    sem_wait(&full); // Chờ cho đến khi có một slot đã đầy trong bộ đệm

    //printf("flushlog()\n");
    sprintf(nullval, "%d", -1);
    for (i = 0; i < count; i++) {
        printf("Slot %i: %s\n", i, logbuf[i]);
        strcpy(logbuf[i], nullval);
    }

    fflush(stdout);

    while ( count != 0) {
      sem_post(&empty);
      count--;
    }
}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main() {
    int i;
    count = 0;
    pthread_t tid[MAX_LOOPS];
    pthread_t lgrid;
    int id[MAX_LOOPS];

    struct _args args;
    args.interval = 500e3; // Interval for flushing logs

    sem_init(&empty, 0, MAX_BUFFER_SLOT); 
    sem_init(&full, 0, 0); 

    pthread_create(&lgrid, NULL, &timer_start, (void *)&args);

    for (i = 0; i < MAX_LOOPS; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, wrlog, (void *)&id[i]);
    }

    for (i = 0; i < MAX_LOOPS; i++)
        pthread_join(tid[i], NULL);

    sleep(5);


    return 0;
}
