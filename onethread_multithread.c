
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#define N 1000
long long sum_multi = 0; 
//long long sum = 0;
pthread_mutex_t mutex;
void* work(void* arg) {
    long n = (long)arg; //may là long và void* cùng kích thước nếu k sẽ lỗi vs int 
    
    volatile long long sum = 0;
    for (long i = 0; i < n; i++) {
        //sum += i;
        pthread_mutex_lock(&mutex); // Khóa mutex
        sum_multi += i;
        pthread_mutex_unlock(&mutex); // Mở khóa mutex
    }
    // 'volatile' để tránh compiler tối ưu bỏ qua vòng lặp
   return NULL;
}

// Lấy timestamp (giây + nanô giây) dưới dạng double (s)
double now_s() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    double t0, t1;

    // ---- Single-thread ----
    // t0 = now_s();
    // long long tong = (long long)(intptr_t)work((void*)N); /// ép kiểu N thành kiểu void* 
    // t1 = now_s();
    // printf("Single-thread time: %.3f s\n", t1 - t0);
    // printf("Tổng = single-thread: %lld\n", tong);
    // ---- Two threads ----
    pthread_t tA, tB,tC;
    pthread_mutex_init(&mutex, NULL);
    t0 = now_s();
    pthread_create(&tA, NULL, work, (void*)(N/3));
    pthread_create(&tB, NULL, work, (void*)(N/3));
    pthread_create(&tC, NULL, work, (void*)(N/3));
    // Chờ cả hai thread xong
    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
    pthread_join(tC, NULL);
    t1 = now_s();
    pthread_mutex_destroy(&mutex);
    printf("Two-thread    time: %.3f s\n", t1 - t0);
    printf("Tổng = two-thread: %lld\n", sum_multi);
    return 0;
}