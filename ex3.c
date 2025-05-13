#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];        // Vùng đệm chia sẻ
int count = 0;                  // Số phần tử hiện có trong buffer

// Khóa bảo vệ vùng chia sẻ và 2 biến điều kiện
pthread_mutex_t mtx;
pthread_cond_t not_empty, not_full;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mtx);
        while (count == BUFFER_SIZE)              // Buffer đầy, chờ consumer lấy bớt
            pthread_cond_wait(&not_full, &mtx);

        buffer[count++] = i;                      // Thêm phần tử mới
        printf("[Producer %d] produce %d, count=%d\n", id, i, count);

        pthread_cond_signal(&not_empty);          // Báo cho consumer biết buffer hiện không trống
        pthread_mutex_unlock(&mtx);

        sleep(1);                                 // Giả lập thời gian sản xuất
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mtx);
        while (count == 0)                        // Buffer rỗng, chờ producer thêm
            pthread_cond_wait(&not_empty, &mtx);

        int v = buffer[--count];                  // Lấy phần tử cuối
        printf("  [Consumer %d] consume %d, count=%d\n", id, v, count);

        pthread_cond_signal(&not_full);           // Báo cho producer biết buffer hiện không đầy
        pthread_mutex_unlock(&mtx);

        sleep(2);                                 // Giả lập thời gian tiêu thụ
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    int pid = 1, cid = 1;

    // 1. Khởi tạo mutex và condition
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full,  NULL);

    // 2. Tạo producer và consumer
    pthread_create(&prod, NULL, producer, &pid);
    pthread_create(&cons, NULL, consumer, &cid);

    // 3. Đợi cả hai kết thúc
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // 4. Dọn dẹp
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}
