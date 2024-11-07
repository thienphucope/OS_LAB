#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main() {
    size_t size = 4096;

    // Ánh xạ một vùng bộ nhớ ẩn danh (không dùng file)
    char *shared_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Ghi dữ liệu vào vùng bộ nhớ
    strcpy(shared_mem, "Hello from the other side!");

    // Đọc dữ liệu từ vùng bộ nhớ
    printf("Data in shared memory: %s\n", shared_mem);

    // Giải phóng vùng bộ nhớ
    munmap(shared_mem, size);
    return 0;
}

