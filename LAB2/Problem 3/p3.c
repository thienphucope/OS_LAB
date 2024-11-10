#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

static int pipefd1[2], pipefd2[2]; // pipefd1: cha -> con, pipefd2: con -> cha

void INIT(void) {
    // Tạo hai đường ống (pipe)
    if (pipe(pipefd1) < 0 || pipe(pipefd2) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

void WRITE_TO_PARENT(void) {
    char msg[] = "alo me a!";
    write(pipefd2[1], msg, strlen(msg) + 1);  // Gửi thông điệp từ con đến cha qua pipefd2
    printf("Child: Sent message to parent!\n");
}

void READ_FROM_PARENT(void) {
    char buffer[100];
    read(pipefd1[0], buffer, sizeof(buffer));  // Đọc thông điệp từ cha qua pipefd1
    printf("Child: Received message from parent: %s\n", buffer);
}

void WRITE_TO_CHILD(void) {
    char msg[] = "alo con!";
    write(pipefd1[1], msg, strlen(msg) + 1);  // Gửi thông điệp từ cha đến con qua pipefd1
    printf("Parent: Sent message to child!\n");
}

void READ_FROM_CHILD(void) {
    char buffer[100];
    read(pipefd2[0], buffer, sizeof(buffer));  // Đọc thông điệp từ con qua pipefd2
    printf("Parent: Received message from child: %s\n", buffer);
}

int main(int argc, char *argv[]) {
    INIT(); // Khởi tạo các pipe
    pid_t pid;
    pid = fork(); // Tạo tiến trình con

    // Đặt hẹn giờ, chương trình sẽ kết thúc sau 10 giây
    alarm(10);

    if (pid == 0) { // Tiến trình con
        while (1) {
            sleep(rand() % 2 + 1);  // Nghỉ ngẫu nhiên từ 1-2 giây
            READ_FROM_PARENT();     // Đọc thông điệp từ cha
            WRITE_TO_PARENT();      // Gửi thông điệp cho cha
        }
    } else { // Tiến trình cha
        while (1) {
            sleep(rand() % 2 + 1);  // Nghỉ ngẫu nhiên từ 1-2 giây
            WRITE_TO_CHILD();       // Gửi thông điệp cho con
            READ_FROM_CHILD();      // Đọc thông điệp từ con
        }
    }

    return 0;
}

