#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Global variables
long long int total_points = 10000000; // Total number of points
int num_threads = 16;                  // Number of threads
long long int *circle_counts;         // Array to store circle counts for each thread

// Thread function to calculate points inside the circle
void *calculate_points(void *arg) {
    long long int points_per_thread = total_points / num_threads;
    long long int circle_count = 0;
    unsigned int seed = (unsigned int)time(NULL) ^ pthread_self();
    
    for (long long int i = 0; i < points_per_thread; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            circle_count++;
        }
    }   
    // Store result in the thread's index
    int thread_id = *(int *)arg;
    circle_counts[thread_id] = circle_count;

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    circle_counts = malloc(num_threads * sizeof(long long int));
    long long int total_circle_points = 0;
    double pi_estimate;
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, calculate_points, &thread_ids[i]);
    }
    // Wait for threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_circle_points += circle_counts[i];
    }
    // Estimate π
    pi_estimate = 4.0 * total_circle_points / total_points;
    // Output results
    printf("Estimated value of pi: %f\n", pi_estimate);
    // Free allocated memory
    free(circle_counts);
    return 0;
}
