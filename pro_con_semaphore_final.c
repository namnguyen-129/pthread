
/* Cách tốt nhất là dùng bộ đếm thơi gian khi buffer rỗng hoặc đầy kiểm tra một thời gian k ai vòng thì break*/
// Cách này thì lặp bao nhiêu lần k quan trọng không sợ bị treo 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE    5
#define N_PRODUCERS    2
#define N_CONSUMERS    5
#define N_ITEM         3  
#define TIMEOUT_SEC    1   

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t sem_empty;  
sem_t sem_full;   
sem_t sem_mutex;  

// Lấy thời điểm hết hạn cho sem_timedwait
void make_timeout(struct timespec *ts, int sec) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += sec;
}

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < N_ITEM; i++) {
        struct timespec ts;
        make_timeout(&ts, TIMEOUT_SEC);
        if (sem_timedwait(&sem_empty, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                printf("Producer %d: timeout because buffer full, exiting at iter %d\n", id, i);
                break;
            }
            perror("sem_timedwait empty");
            exit(1);
        }
        // mutex
        sem_wait(&sem_mutex);

        int item = id * 100 + i;
        buffer[in] = item;
        printf("Producer %d wrote %d at buffer[%d]\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&sem_mutex);
        sem_post(&sem_full);
        usleep(50000);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < N_ITEM; i++) {
        struct timespec ts;
        make_timeout(&ts, TIMEOUT_SEC);
        if (sem_timedwait(&sem_full, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                printf("Consumer %d: timeout because buffer empty, exiting at iter %d\n", id, i);
                break;
            }
            perror("sem_timedwait full");
            exit(1);
        }
        sem_wait(&sem_mutex);

        int item = buffer[out];
        printf("Consumer %d read %d from buffer[%d]\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&sem_mutex);
        sem_post(&sem_empty);
        usleep(80000);
    }
    return NULL;
}

int main() {
    pthread_t prod_tid[N_PRODUCERS], cons_tid[N_CONSUMERS];
    int pid[N_PRODUCERS], cid[N_CONSUMERS];

    // Khởi tạo semaphore
    sem_init(&sem_empty, 0, BUFFER_SIZE);
    sem_init(&sem_full,  0, 0);
    sem_init(&sem_mutex, 0, 1);

    // Tạo producer
    for (int i = 0; i < N_PRODUCERS; i++) {
        pid[i] = i + 1;
        pthread_create(&prod_tid[i], NULL, producer, &pid[i]);
    }
    // Tạo consumer
    for (int i = 0; i < N_CONSUMERS; i++) {
        cid[i] = i + 1;
        pthread_create(&cons_tid[i], NULL, consumer, &cid[i]);
    }

    // Join
    for (int i = 0; i < N_PRODUCERS; i++) pthread_join(prod_tid[i], NULL);
    for (int i = 0; i < N_CONSUMERS; i++) pthread_join(cons_tid[i], NULL);

    // Destroy
    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    sem_destroy(&sem_mutex);
    return 0;
}
