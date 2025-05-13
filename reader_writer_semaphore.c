#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h> 
#define Num_reader 5
#define Num_writer 2
#define N_iters 10 
int shared_data = 5;
int read_count = 0;
int write_count = 0;
/*              semaphore basic luân phiên đọc ghi nhưng chỉ được khi số reader = writer 
vì reader phải gọi writer và writer đó mới gọi reader tiếp do khởi tạo binary semaphore           */
// sem_t wr,mutex;
// void* reader(void* args){
//     int id = *(int*)args;
//     for(int i = 0; i < N_iters; i++){
//         sem_wait(&mutex); // giảm read về 0 và đọc thì lần tiếp theo sẽ đến lượt của writer luôn
//         printf("Reader %d read data: %d\n", id, shared_data);
//         sem_post(&wr);
//     }
// }
// void* writer(void* args){
//     int id = *(int*)args;
//     for(int i = 0; i < N_iters; i++){
//         sem_wait(&wr); // writer vào ghi thì reader không được vào
//         shared_data = id * 10 + i;
//         printf("Writer %d write data: %d\n", id, shared_data);
//         sem_post(&mutex); // mở mutex để cho phép reader vào đọc
//     }
// }
// /*              Semaphore                  */
sem_t wr, mutex;
void* reader(void *args){
    int id = *(int*) args;
    for(int i = 0; i < N_iters + 1; i++){
        sem_wait(&mutex);
        read_count ++;
        if(read_count == 1)
            sem_wait(&wr); // nếu là reader đầu thì đưa write về 0 để block các writer 
        sem_post(&mutex); // trao quyền cho các reader khác được vào đọc 
    /*       Đây là vùng CS mà các reader được phép truy cập            */
        printf("Reader %d read data: %d\n", id, shared_data);
        usleep(500); // thời gian đọc của mỗi reader
    /*        Khi có một reader rời đi thì phải truy cập read_count(shared) nên phải khóa             */
        sem_wait(&mutex);
        read_count --;
        if(read_count == 0)
            sem_post(&wr); // nếu là reader cuối thì cho write lên 1 cho phép writer vào 
        sem_post(&mutex); // trao quyền cho các reader khác được vào đọc
        usleep(2000);
    }
}
void* writer(void* args){
    int id = *(int*) args;
    for(int i = 0 ; i < N_iters; i ++){
        sem_wait(&wr); // chặn cả writer và reader vào
        /*       Đây là vùng CS mà writer được phép truy cập để ghi            */
        shared_data = id * 10 + i;
        printf("Writer %d write data: %d\n", id, shared_data);
        sem_post(&mutex); // mở mutex để cho phép reader vào đọc
        usleep(1000);
        sem_wait(&mutex); // khóa mutex để k cho reader vào nữa
        sem_post(&wr);
        usleep(2000); 
    }
}
int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&wr, 0, 1);
    pthread_t readerid[Num_reader], writerid[Num_writer];
    int rid[Num_reader];
    int wid[Num_writer];
    for(int i = 0; i < Num_reader; i ++){
        rid[i] = i; // Khởi tạo giá trị cho mảng rid
        pthread_create(&readerid[i], NULL, reader, &rid[i]);
    }
    for(int i = 0; i < Num_writer; i ++){
        wid[i] = i; // Khởi tạo giá trị cho mảng wid
        pthread_create(&writerid[i], NULL, writer, &wid[i]);
    }
    for(int i = 0; i < Num_reader; i ++){
        pthread_join(readerid[i], NULL);
    }
    for(int i = 0; i < Num_writer; i ++){
        pthread_join(writerid[i], NULL);
    }
    sem_destroy(&mutex);
    sem_destroy(&wr);
}

