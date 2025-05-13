/* Producer-consumer */ 
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h> // dùng cho usleep

sem_t empty, full, mutex; 
// empty: số ô còn trống trong buffer; full: số ô đã được lấp đầy; 
//mutex: đảm bảo chỉ một thread(producer or consumer) truy cập vào buffer 
#define Buffer_size 5
#define N_item 3 // số  item mà mỗi producer tạo ra 
#define Num_producer 4
#define Num_consumer 2 // consumer bao giờ cũng phải nhiều hơn producer mới được nếu nhiều hơn thì vòng for của consumer phải chạy N_item 
// nếu không sẽ bị treo do k có gì để consumer tiêu thụ mà lúc ấy producer đã ngừng rồi 
int total_item = Num_producer * N_item; // tổng số item mà producer tạo ra   
int buffer[Buffer_size];
int in = 0; int out = 0; // ban đầu thì buffer rỗng cả in, out đều = 0(sau in đóng vai trò tail, out là head)
void* producer(void* args){
    int id = *(int*)args; // lấy giá trị lưu trong ô nhớ của con trỏ args 
    for(int i = 0; i < Num_consumer * N_item / Num_producer; i ++){ 
// Cần phải để  vậy nếu không producer mà nhiều hơn consumer, nó cũng sẽ sản xuất thừa đến khi buffer đầy mà
// vẫn k còn consumer nào vào đọc thì sẽ bị chết ở sem_wait(&empty)
        sem_wait(&empty);// kiểm tra empty > 0(còn chỗ để ghi vào là producer sẽ vào); ban đầu empty = 5 và giảm về 
        sem_wait(&mutex);// chỉ một producer được vào ghi, mutex về 0
        buffer[in] = (id+i)*10;
        printf("Producer %d write %d at buffer[%d]\n", id, buffer[in], in);
        in = (in + 1) % Buffer_size; // Đchia để  inquay vòng về đầu 
        sem_post(&mutex);// unlock mutex để producer or consumer khác vào mutex = 1
        sem_post(&full); // lúc đầu full = 0 và khi producer ghi vào tì full ++
        //usleep(1000); 
// sleep thì producer và consumer sẽ xen kẽ bất kỳ nếu không thì producer điền đầy hết(or qua hết N_item) rồi mới sang consumer
    }
}
// cả in và out sau một lượt đều cộng lên 1 để đọc ghi vào vị trí tiếp theo (Num_producer * N_item)/Num_consumer
void* consumer(void* args){
    int id = *(int*) args;
    for(int i =0; i < N_item ; i ++){ 
// để thế này để đảm bảo không bị treo trong trường hợp nhiều producer và consumer, Nếu chỉ để N_item 
// Producer 0 ghi 3 lần và 1 ghi 2 lần rồi consumer 0 tiêu thụ 3 item
// tiếp theo đến lươt producer 1 ghi nốt lần thứ 3 => buffer có 3 và dừng chuyển sang phiên consumer 
// consumer 1 tiêu thụ nốt 3 item là buffer hết, consumer cuối bị treo mãi vì k còn gì để đọc thì sẽ chết ở sem_wait full
// trừ khi nhiều producer hơn
        sem_wait(&full);// kiểm tra full > 0 buffer có dữ liệu vào thì consumer sẽ vào và giảm số ô có dữ liệu đi một 
        sem_wait(&mutex);
        int tmp = buffer[out];
        printf("Consumer %d read %d from buffer[%d]\n", id, tmp, out);
        out = (out + 1) % Buffer_size; // Đảm bảo out luôn nằm trong phạm vi [0, Buffer_size - 1]
        sem_post(&mutex);
        sem_post(&empty); // empty ++ vì consumer đã lấy đi một ô dữ liệu 
        //usleep(1000); // thì producer và consumer sẽ xen kẽ

    }
}
int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, Buffer_size); // ban đầu buffer rỗng
    sem_init(&full,0 , 0);// ban đầu không có dũe liệu
    pthread_t proid[Num_producer], conid[Num_consumer];
    int pid[Num_producer]; // mảng để lưu ID của các producer
    int cid[Num_consumer]; // mảng để lưu ID của các consumer
    for(int i = 0; i < Num_producer; i ++){
        pid[i] = i; 
        pthread_create(&proid[i], NULL, producer, &pid[i]);
    }
    for(int i = 0; i < Num_consumer; i ++){
        cid[i] = i;
        pthread_create(&conid[i], NULL, consumer, &cid[i]);
    }
    for(int i = 0; i < Num_producer; i ++){
        pthread_join(proid[i], NULL); // chờ producer xong
    }
    for(int i = 0; i < Num_consumer; i ++){
        pthread_join(conid[i], NULL); // chờ consumer xong
    }
    
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

}