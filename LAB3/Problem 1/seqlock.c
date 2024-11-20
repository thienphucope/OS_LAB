#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "seqlock.h"
// Cấu trúc seqlock đã định nghĩa ở trên
typedef struct {
    pthread_seqlock_t lock;
    int shared_data; // Dữ liệu được bảo vệ bởi seqlock
} shared_resource_t;

shared_resource_t resource;

void* reader_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < 5; ++i) {
        if (pthread_seqlock_rdlock(&resource.lock)) {
            printf("Reader %d: Read data = %d (sequence = %u)\n", id, resource.shared_data, resource.lock.sequence);
            pthread_seqlock_rdunlock(&resource.lock);
        } else {
            printf("Reader %d: Waiting for writer...\n", id);
        }
        usleep(100000); // Nghỉ 100ms để mô phỏng đọc dữ liệu
    }
    return NULL;
}

void* writer_thread(void* arg) {
    for (int i = 0; i < 5; ++i) {
        if (pthread_seqlock_wrlock(&resource.lock)) {
            resource.shared_data += 10; // Cập nhật dữ liệu
            printf("Writer: Updated data to %d (sequence = %u)\n", resource.shared_data, resource.lock.sequence);
            pthread_seqlock_wrunlock(&resource.lock);
        } else {
            printf("Writer: Waiting for readers...\n");
        }
        usleep(200000); // Nghỉ 200ms để mô phỏng ghi dữ liệu
    }
    return NULL;
}

int main() {
    pthread_t readers[3], writer;
    int reader_ids[3] = {1, 2, 3};

    // Khởi tạo seqlock
    pthread_seqlock_init(&resource.lock);
    resource.shared_data = 0;

    // Tạo các luồng reader
    for (int i = 0; i < 3; ++i) {
        if (pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]) != 0) {
            perror("Failed to create reader thread");
            exit(EXIT_FAILURE);
        }
    }

    // Tạo luồng writer
    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("Failed to create writer thread");
        exit(EXIT_FAILURE);
    }

    // Chờ các luồng reader kết thúc
    for (int i = 0; i < 3; ++i) {
        pthread_join(readers[i], NULL);
	pthread_cancel(readers[i]);
    }

    // Chờ luồng writer kết thúc
    pthread_join(writer, NULL);
    pthread_cancel(writer);
    printf("Test completed successfully!\n");
    return 0;
}
