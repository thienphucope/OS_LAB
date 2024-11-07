#include <stdio.h>
#include <stdlib.h>
//time ./p21 <n>
// Hàm tính tổng từ 1 đến n
long long sum_serial(int n) {
    long long sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return -1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "n must be a positive integer.\n");
        return -1;
    }

    // Tính tổng từ 1 đến n
    long long total_sum = sum_serial(n);
    printf("Total sum (serial) from 1 to %d: %lld\n", n, total_sum);

    return 0;
}

