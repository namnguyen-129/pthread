#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t con;
pthread_cond_t pro; 
#define BUFFER_SIZE 5
#define N_item 10
int buffer[BUFFER_SIZE];
int count = 0;
int in =0; int out = 0;
void *producer(void* args){
    int id = * (int*)args;
    for(int i = 0; i < N_item; i ++){
        pthread_mutex_lock(&mutex);
        while(count == BUFFER_SIZE){ //nếu đầy thì producer phải đợi ở hàng đợi
            pthread_cond_wait(&pro, &mutex);
// vừa unlock mutex để thread khác(consumer) vào lấy item đi vưa block thread của func producer hiện tại vào ghi 
        }
        // ra đến đây tức là conut <  BUFFER_SIZE, cồn chỗ để ghi thì ghi 
        
        buffer[in] = (i+1)*10;
        
        printf("Producer %d write %d at buffer[%d]\n", id, buffer[in], in);
        in = (in+1) % BUFFER_SIZE;
        count++;
         // đánh thức consumer đang đợi ở hàng đợi rằng có dữ liệu đây đến lấy đi
        pthread_cond_signal(&con);
        pthread_mutex_unlock(&mutex);   
        usleep(1000);
    }
}
void *consumer(void* args){
    int id = * (int*)args;
    for(int i = 0; i < N_item; i ++){// chưa rỗng thì consumer được vào
        pthread_mutex_lock(&mutex);
        while(count == 0){// buffer rỗng thì phải block thread này tức consumer vào hàng đợi
            pthread_cond_wait(&con, &mutex);
        }
        int tmp = buffer[out];
        
        printf("Consumer %d read %d from buffer[%d]\n", id, tmp, out);
        out = (out+1) % BUFFER_SIZE;
        count--;
        pthread_cond_signal(&pro); 
// đánh thức producer đang đợi ở hàng đợi rằng có chỗ trống để ghi vào
// Nếu mà không đánh thức pro dậy thì sẽ rơi vào deadlock vì k có dữ liệu ghi vào 
        pthread_mutex_unlock(&mutex);
        usleep(1000);
        
    }
}
int main(){
    pthread_t proid, conid;
    int pid = 1; int cid = 2;
    pthread_create(&proid, NULL, producer, &pid);
    pthread_create(&conid, NULL, consumer, &cid);
    pthread_join(proid, NULL);
    pthread_join(conid, NULL);

}

/*      MULTIPRODUCER - MULTICONSUMER chưa chạy ổn   */
// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <time.h>
// pthread_mutex_t mutex;
// pthread_cond_t con;
// pthread_cond_t pro; 
// #define BUFFER_SIZE 5
// #define N_item 10
// #define Num_producer    2
// #define Num_consumer   5
// #define TIMEOUT_SEC 1
// int buffer[BUFFER_SIZE];
// int count = 0;
// int in = 0; int out = 0;

// void make_timeout(struct timespec *ts) {
//     clock_gettime(CLOCK_REALTIME, ts);
//     ts->tv_sec += TIMEOUT_SEC;
// }
// void *producer(void* args){
//     int id = * (int*)args;
//     for(int i = 0; i < N_item; i ++){
//         struct timespec ts;
//         pthread_mutex_lock(&mutex);
//         while(count == BUFFER_SIZE){ //nếu đầy thì producer phải đợi ở hàng đợi 
//             make_timeout(&ts);
//             int rc = pthread_cond_timedwait(&pro, &mutex, &ts);
//             if (rc == TIMEOUT_SEC) {
//                 printf("Producer %d: timeout waiting for space, exiting at item %d\n", id, i);
//                 pthread_mutex_unlock(&mutex);
//                 return NULL;
//             }

            
//             // vừa unlock mutex để thread khác(consumer) vào lấy item đi vưa block thread của func producer hiện tại vào ghi 
//         }
//         // ra đến đây tức là conut <  BUFFER_SIZE, cồn chỗ để ghi thì ghi 
        
//         buffer[in] = (i+1)*10;
        
//         printf("Producer %d write %d at buffer[%d]\n", id, buffer[in], in);
//         usleep(1000);
//         in = (in+1) % BUFFER_SIZE;
//         count++;
//          // đánh thức consumer đang đợi ở hàng đợi rằng có dữ liệu đây đến lấy đi
//         pthread_cond_signal(&con);
//         pthread_mutex_unlock(&mutex);   
//         usleep(1000);
//     }
// }
// void *consumer(void* args){
//     int id = * (int*)args;
//     for(int i = 0; i < N_item; i ++){// chưa rỗng thì consumer được vào
//         struct timespec ts;
//         pthread_mutex_lock(&mutex);
//         while(count == 0){// buffer rỗng thì phải block thread này tức consumer vào hàng đợi
//             make_timeout(&ts);
//             int rc = pthread_cond_timedwait(&con, &mutex, &ts);
//             if (rc == TIMEOUT_SEC) {
//                 printf("Consumer %d: timeout waiting for data, exiting at iter %d\n", id, i);
//                 pthread_mutex_unlock(&mutex);
//                 return NULL;
//             }
            
//         }
//         int tmp = buffer[out];
        
//         printf("Consumer %d read %d from buffer[%d]\n", id, tmp, out);
//         out = (out+1) % BUFFER_SIZE;
//         count--;
//         pthread_cond_signal(&pro); 
//         // đánh thức producer đang đợi ở hàng đợi rằng có chỗ trống để ghi vào
//         // Nếu mà không đánh thức pro dậy thì sẽ rơi vào deadlock vì k có dữ liệu ghi vào 
//         pthread_mutex_unlock(&mutex);
//         usleep(1000);
        
//     }
// }
// int main(){
//     pthread_mutex_init(&mutex, NULL);
//     pthread_cond_init(&con, NULL);
//     pthread_cond_init(&pro, NULL);
//     pthread_t proid[Num_producer], conid[Num_consumer];
//     int pid[Num_producer]; // mảng để lưu ID của các producer
//     int cid[Num_consumer]; // mảng để lưu ID của các consumer

//     for(int i = 0; i < Num_producer; i ++){
//         pid[i] = i; 
//         pthread_create(&proid[i], NULL, producer, &pid[i]);
//     }
//     for(int i = 0; i < Num_consumer; i ++){
//         cid[i] = i;
//         pthread_create(&conid[i], NULL, consumer, &cid[i]);
//     }
//     for(int i = 0; i < Num_producer; i ++){
//         pthread_join(proid[i], NULL); // chờ producer xong
//     }
//     for(int i = 0; i < Num_consumer; i ++){
//         pthread_join(conid[i], NULL); // chờ consumer xong
//     }
//     pthread_mutex_destroy(&mutex);
//     pthread_cond_destroy(&con);
//     pthread_cond_destroy(&pro);
// }

