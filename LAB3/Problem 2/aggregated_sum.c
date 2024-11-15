#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct range {
    int start;
    int end;
};

int *array;           // The array of integers to sum
int *sumbuff;         // Buffer to hold partial sums for each thread
int arrsz, tnum;      // Array size and number of threads

int generate_array_data(int *buf, int array_size, int seednum) {
    srand(seednum);  // Initialize the random number generator with the given seed

    for (int i = 0; i < array_size; i++) {
        buf[i] = rand();  // Generate a random integer and store it in the buffer
    }

    return 0;  // Returning 0 to indicate success
}

void* sum_worker(void *arg) {
    struct range *idx_range = (struct range*) arg;
    int sum = 0;
    
    // Calculate sum in the specified range
    for (int i = idx_range->start; i <= idx_range->end; i++) {
        sum += array[i];
    }
    
    // Store the sum in the global sum buffer
    sumbuff[idx_range->start / (arrsz / tnum)] = sum;
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s arrsz tnum [seednum]\n", argv[0]);
        return 1;
    }
    
    arrsz = atoi(argv[1]);
    tnum = atoi(argv[2]);
    int seednum = (argc > 3) ? atoi(argv[3]) : 0;

    // Allocate memory for the array and sum buffer
    array = (int*)malloc(arrsz * sizeof(int));
    sumbuff = (int*)malloc(tnum * sizeof(int));

    // Generate the array with the provided seed
    generate_array_data(array, arrsz, seednum);

    pthread_t threads[tnum];
    struct range thread_ranges[tnum];
    int range_size = arrsz / tnum;

    // Initialize each thread's range
    for (int i = 0; i < tnum; i++) {
        thread_ranges[i].start = i * range_size;
        thread_ranges[i].end = (i == tnum - 1) ? (arrsz - 1) : ((i + 1) * range_size - 1);
        
        // Create each thread
        pthread_create(&threads[i], NULL, sum_worker, &thread_ranges[i]);
    }

    // Join all threads
    for (int i = 0; i < tnum; i++) {
        pthread_join(threads[i], NULL);
    }

    // Aggregate all partial sums
    int total_sum = 0;
    for (int i = 0; i < tnum; i++) {
        total_sum += sumbuff[i];
    }

    printf("Total sum: %d\n", total_sum);

    // Free allocated memory
    free(array);
    free(sumbuff);

    return 0;
}
