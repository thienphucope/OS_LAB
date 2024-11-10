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
    pid_t pid = fork();

   if (pid == 0)
   {
	   printf("%s\n", (char*) shared_mem);
   }
   else
   {
	   strcpy(shared_mem, "Hello from the other side");
   }
 
    // Giải phóng vùng bộ nhớ
    munmap(shared_mem, size);
    return 0;
}

