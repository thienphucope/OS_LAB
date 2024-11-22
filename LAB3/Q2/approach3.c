#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

// Shared variables
long long total_points;  // Total number of points to generate
long long points_inside = 0;  // Shared count of points inside circle
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for synchronization

// Thread argument structure
typedef struct {
    int thread_id;
    int num_threads;
} ThreadArg;

// Get random double between 0 and 1
double get_random() {
    return (double)rand() / RAND_MAX;
}

// Thread function
void* monte_carlo_pi(void* arg) {
    ThreadArg* thread_arg = (ThreadArg*)arg;
    long long points_per_thread = total_points / thread_arg->num_threads;
    long long local_points_inside = 0;  // Local counter
    
    // Calculate points for this thread
    for (long long i = 0; i < points_per_thread; i++) {
        double x = get_random();
        double y = get_random();
        
        // Check if point is inside circle
        if (x*x + y*y <= 1.0) {
            local_points_inside++;
        }
        
        // Update shared counter periodically to reduce mutex overhead
        if (i % 1000 == 0 && local_points_inside > 0) {
            pthread_mutex_lock(&mutex);
            points_inside += local_points_inside;
            pthread_mutex_unlock(&mutex);
            local_points_inside = 0;
        }
    }
    
    // Add remaining local points
    if (local_points_inside > 0) {
        pthread_mutex_lock(&mutex);
        points_inside += local_points_inside;
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

// Main function to run the simulation
double run_simulation(long long num_points, int num_threads) {
    total_points = num_points;
    points_inside = 0;
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Create threads
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadArg* thread_args = (ThreadArg*)malloc(num_threads * sizeof(ThreadArg));
    
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].num_threads = num_threads;
        pthread_create(&threads[i], NULL, monte_carlo_pi, &thread_args[i]);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Calculate pi
    double pi = 4.0 * points_inside / total_points;
    
    // Calculate execution time
    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + 
                       (end.tv_usec - start.tv_usec) / 1000000.0;
    
    // Cleanup
    free(threads);
    free(thread_args);
    
    printf("Threads: %d, Points: %lld, Pi: %.6f, Time: %.6f seconds\n", 
           num_threads, total_points, pi, time_taken);
           
    return time_taken;
}

int main() {
    srand(time(NULL));
    long long num_points = 10000000;  // 10M points
    
    // Run with different thread counts and measure speedup
    double base_time = run_simulation(num_points, 1);  // Base time with 1 thread
    
    printf("\nSpeedup Analysis:\n");
    // Test with different thread counts (2 to 32 threads)
    for (int threads = 2; threads <= 32; threads *= 2) {
        double time = run_simulation(num_points, threads);
        double speedup = base_time / time;
        printf("Threads: %d, Speedup: %.2fx\n", threads, speedup);
    }
    
    return 0;
}