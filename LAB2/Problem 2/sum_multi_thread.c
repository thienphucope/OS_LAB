#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//time ./p22 <threads> <n>
// Cấu trúc lưu trữ thông tin cho mỗi luồng
typedef struct {
    int start;
    int end;
    long long partial_sum;
} ThreadData;

// Hàm mà mỗi luồng sẽ thực hiện để tính tổng phần của nó
void* compute_sum(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->partial_sum = 0;
    for (int i = data->start; i <= data->end; i++) {
        data->partial_sum += i;
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <numThreads> <n>\n", argv[0]);
        return -1;
    }

    int numThreads = atoi(argv[1]);
    int n = atoi(argv[2]);
    if (numThreads <= 0 || n <= 0) {
        fprintf(stderr, "numThreads and n must be positive integers.\n");
        return -1;
    }

    // Khởi tạo mảng dữ liệu cho các luồng
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    // Chia dãy số thành các phần cho các luồng
    int range_per_thread = n / numThreads;
    int remainder = n % numThreads; // Phần dư

    int current_start = 1;
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = current_start;
        threadData[i].end = current_start + range_per_thread - 1;
        if (remainder > 0) {
            threadData[i].end += 1;
            remainder--;
        }
        current_start = threadData[i].end + 1;
        
        // Tạo luồng để tính toán
        pthread_create(&threads[i], NULL, compute_sum, &threadData[i]);
    }

    // Đợi các luồng hoàn thành và thu thập kết quả
    long long total_sum = 0;
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += threadData[i].partial_sum;
    }

    printf("Total sum (multithread) from 1 to %d: %lld\n", n, total_sum);

    return 0;
}

