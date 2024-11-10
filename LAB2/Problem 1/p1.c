#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SHM_KEY 0x1234

// Hàm tính trung bình rating từ file
float compute_average(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    int userID, movieID, rating, timeStamp;
    int totalRatings = 0;
    int count = 0;
    while (fscanf(file, "%d\t%d\t%d\t%d\n", &userID, &movieID, &rating, &timeStamp) != EOF) {
        totalRatings += rating;
        count++;
    }
    fclose(file);

    if (count == 0) return 0;
    return (float)totalRatings / count;
}

int main() {
    int shmid;
    float *shared_memory;

    // Tạo vùng nhớ chung
    shmid = shmget(SHM_KEY, 2 * sizeof(float), IPC_CREAT | 0666);

    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Gắn vùng nhớ chung vào tiến trình hiện tại
    shared_memory = (float*) shmat(shmid, NULL, 0);
    if (shared_memory == (float*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Tiến trình con 1
        shared_memory[0] = compute_average("movie-100k_1.txt");
        printf("Child 1 computed average: %.2f\n", shared_memory[0]);
        shmdt(shared_memory);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Tiến trình con 2
        shared_memory[1] = compute_average("movie-100k_2.txt");
        printf("Child 2 computed average: %.2f\n", shared_memory[1]);
        shmdt(shared_memory);
        exit(0);
    }
 
    // Đợi cả hai tiến trình con hoàn thành
    wait(NULL);
    wait(NULL);
 
    float* result = (float*) shmat(shmid, NULL, 0);
    if (result == (float*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }


    // Tiến trình cha thu thập kết quả
    printf("Average rating from file 1: %.2f\n", result[0]);
    printf("Average rating from file 2: %.2f\n", result[1]);

    // Hủy vùng nhớ chung
    shmdt(result);
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

