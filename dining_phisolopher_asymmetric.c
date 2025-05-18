/*Người chẵn thì lấy đũa trái(i) trước rồi đến phải(i+1) còn người bên lẻ thì lấy phải trước rồi đến trái
=> nếu hai người ngồi cạnh nhau thì có thể họ sẽ cùng chờ cùng một chiếc đũa và khi người này cầm thì ngừoi kia
sẽ đợi chứ không xuất hiện vòng lặp P0 đợi đúng chiếc mà P1 cầm,....*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define Num 5
int running = 1;
int eaten[Num] = {0}; //kiểm tra xem ai ăn rồi
int count = 0; 
pthread_mutex_t chopstick[Num];

void* philosopher(void* arg) {
    int i = *(int*)arg;
    int left  = i;
    int right = (i + 1) % Num;

    while (running) {
        printf("Philosopher %d is thinking\n", i);
        sleep(1);

        if (i % 2 == 0) {
            pthread_mutex_lock(&chopstick[left]);
            pthread_mutex_lock(&chopstick[right]);
        } else {
            pthread_mutex_lock(&chopstick[right]);
            pthread_mutex_lock(&chopstick[left]);
        }

        // EAT
        printf("Philosopher %d is eating\n", i);
        sleep(2);
        if(eaten[i] == 0){
            eaten[i] = 1;
            printf("Philosopher %d ate\n", i);
            count ++;
        }
        if(count == Num){
            running = 0;
            printf("All philosophers have eaten\n");
            // pthread_mutex_unlock(&chopstick[left]);
            // pthread_mutex_unlock(&chopstick[right]);
        }
        // Ăn xong thì buông đũa xuống
        pthread_mutex_unlock(&chopstick[left]);
        pthread_mutex_unlock(&chopstick[right]);
    }
    return NULL;
}

int main() {
    pthread_t tid[Num];
    int id[Num];

    for (int i = 0; i < Num; i++)
        pthread_mutex_init(&chopstick[i], NULL);

    for (int i = 0; i < Num; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &id[i]);
    }

    for (int i = 0; i < Num; i++)
        pthread_join(tid[i], NULL);

    return 0;
}