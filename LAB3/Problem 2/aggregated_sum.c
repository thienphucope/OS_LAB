#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 16

// Global variables
int *array;               // The array of integers
int array_size;           // The size of the array
int total_sum = 0;        // The total sum of the array (shared resource)
pthread_mutex_t sum_lock; // Mutex to protect the shared resource (total_sum)

// Structure to define the range each thread will process
struct range {
    int start;
    int end;
};

// Function to generate random array values
int generate_array_data(int *buf, int array_size, int seednum) {
    srand(seednum);
    for (int i = 0; i < array_size; i++) {
        buf[i] = rand() % 100;  // Generate random values between 0 and 99
    }
    return 0;
}

// Sum worker function to be run by each thread
void* sum_worker(void* arg) {
    struct range *r = (struct range*)arg;
    int local_sum = 0;

    // Calculate sum for the given range
    for (int i = r->start; i <= r->end; i++) {
        local_sum += array[i];
    }

    // Lock the shared resource (total_sum) before updating it
    pthread_mutex_lock(&sum_lock);
    total_sum += local_sum;
    pthread_mutex_unlock(&sum_lock);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <array_size> <num_threads> [seednum]\n", argv[0]);
        return -1;
    }

    // Parse command line arguments
    array_size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int seednum = (argc > 3) ? atoi(argv[3]) : 0;

    // Validate the number of threads
    if (num_threads <= 0 || num_threads > MAX_THREADS) {
        printf("Error: Number of threads should be between 1 and %d.\n", MAX_THREADS);
        return -1;
    }

    // Allocate memory for the array
    array = (int*)malloc(array_size * sizeof(int));

    // Initialize the mutex
    pthread_mutex_init(&sum_lock, NULL);

    // Generate random array data
    generate_array_data(array, array_size, seednum);

    // Create threads and assign ranges
    pthread_t threads[num_threads];
    struct range thread_ranges[num_threads];

    int range_size = array_size / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_ranges[i].start = i * range_size;
        thread_ranges[i].end = (i == num_threads - 1) ? (array_size - 1) : ((i + 1) * range_size - 1);
        
        // Create the thread to compute sum for the range
        if (pthread_create(&threads[i], NULL, sum_worker, &thread_ranges[i]) != 0) {
            perror("Failed to create thread");
            return -1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the total sum
    printf("Total sum of array = %d\n", total_sum);

    // Cleanup
    pthread_mutex_destroy(&sum_lock);
    free(array);

    return 0;
}
