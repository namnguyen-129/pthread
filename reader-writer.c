#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 3
#define NUM_WRITERS 2
#define N_ITERS     10

// Shared data
int shared_data = 0;


sem_t sem_reader; // Ban đầu khởi tạo là số reader   
sem_t sem_writer[NUM_WRITERS]; // Separate sem for each writer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
int count_reader = 0;    
int next_writer = 0;      

void* reader(void* arg) {
    int id = *(int*)arg;
    for (int cycle = 0; cycle < N_ITERS; cycle++) {
        sem_wait(&sem_reader);  
    
        // Read section
        printf("[Reader %d] read shared_data = %d (cycle %d)\n", id, shared_data, cycle);
        usleep(5000);

        // Barrier: last reader signals writer
        pthread_mutex_lock(&mutex);
        count_reader++;
        if (count_reader == NUM_READERS) {
            int w = next_writer;  // get writer to wake
            next_writer = (next_writer + 1) % NUM_WRITERS;
            count_reader = 0;    // reset for next cycle
            sem_post(&sem_writer[w]);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg; // 0..NUM_WRITERS-1
    for (int cycle = 0; cycle < N_ITERS; cycle++) {
        sem_wait(&sem_writer[id]); // Wait for its turn

        // Write section
        shared_data = id * 10 + cycle;
        printf("[Writer %d] wrote shared_data = %d (cycle %d)\n", id+1, shared_data, cycle);
        usleep(10000);

        // Release all readers for next cycle
        for (int j = 0; j < NUM_READERS; j++) {
            sem_post(&sem_reader);
        }
    }
    return NULL;
}

int main() {
    pthread_t rtid[NUM_READERS], wtid[NUM_WRITERS];
    int rid[NUM_READERS], wid[NUM_WRITERS];

    // Initialize semaphores
    sem_init(&sem_reader, 0, NUM_READERS);
    for (int i = 0; i < NUM_WRITERS; i++) {
        sem_init(&sem_writer[i], 0, 0);
    }

    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        rid[i] = i + 1;
        pthread_create(&rtid[i], NULL, reader, &rid[i]);
    }
    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        wid[i] = i;
        pthread_create(&wtid[i], NULL, writer, &wid[i]);
    }

    // Wait for threads
    for (int i = 0; i < NUM_READERS; i++) pthread_join(rtid[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++) pthread_join(wtid[i], NULL);

    // Cleanup
    sem_destroy(&sem_reader);
    for (int i = 0; i < NUM_WRITERS; i++) sem_destroy(&sem_writer[i]);
    pthread_mutex_destroy(&mutex);

    return 0;
}