#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 5

// Shared log buffer
char *logbuf[MAX_BUFFER_SLOT];
int buffer_count = 0;  // Number of entries in the buffer

// Mutex and condition variables for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

// Function to write log into the buffer
int wrlog(char *new_data) {
    pthread_mutex_lock(&mutex);

    // Wait if buffer is full
    while (buffer_count == MAX_BUFFER_SLOT) {
        pthread_cond_wait(&cond_not_full, &mutex);
    }

    // Add the new log to the buffer
    logbuf[buffer_count] = strdup(new_data);
    buffer_count++;

    pthread_cond_signal(&cond_not_empty); // Signal that buffer is not empty
    pthread_mutex_unlock(&mutex);
    return 0;
}

// Function to flush the buffer
int flushlog() {
    pthread_mutex_lock(&mutex);

    // Wait if buffer is empty
    while (buffer_count == 0) {
        pthread_cond_wait(&cond_not_empty, &mutex);
    }

    // Print and clear the buffer
    printf("Flushing log:\n");
    for (int i = 0; i < buffer_count; i++) {
        printf("Slot %d: %s\n", i, logbuf[i]);
        free(logbuf[i]); // Free allocated memory
        logbuf[i] = NULL;
    }
    buffer_count = 0;

    pthread_cond_signal(&cond_not_full); // Signal that buffer is not full
    pthread_mutex_unlock(&mutex);
    return 0;
}

// Periodically flush log in a separate thread
void *flush_thread(void *arg) {
    while (1) {
        sleep(5);  // Flush every 5 seconds
        flushlog();
    }
    return NULL;
}

int main() {
    // Initialize the flush thread
    pthread_t tid;
    pthread_create(&tid, NULL, flush_thread, NULL);

    // Simulate multiple threads writing to the log
    for (int i = 0; i < 30; i++) {
        char log[MAX_LOG_LENGTH];
        snprintf(log, sizeof(log), "%d", i);
        wrlog(log);
        usleep(500000); // Simulate delay between writes (0.5s)
    }

    // Wait for the flush thread to finish (it won't in this example)
    pthread_join(tid, NULL);
    return 0;
}
