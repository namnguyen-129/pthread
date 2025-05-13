// /*     2 luồng A, B cùng truy cập vào tăng counter 100 lần => expect 200*/
#include<stdint.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

#define NUM_iter 100
/*     2 luồng A, B cùng truy cập vào tăng counter 100 lần => expect 200*/
sem_t sem;
int counter = 0; // tài nguyên dùng chung
void* sema(void* args){
    printf("TID: %lu\n", pthread_self());
    for(int i = 0; i < NUM_iter; i++){
        sem_wait(&sem);
        counter ++;
        sem_post(&sem);
    }
}
int main(){
    sem_init(&sem, 0 ,1);//binary semaphore, phải khởi tạo trước tạo pthread_create nếu không sẽ bị dead lock và treo
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, sema,NULL);// ghi ID vào ô nhớ của biến TID1-> TID1 = ID thread
    pthread_create(&tid2, NULL, sema, NULL);
    
    pthread_join(tid1,NULL); // chờ thread có ID tid1 xong
   // phải đợi các thread xong hết nếu k sẽ ra không mong muốn
    pthread_join(tid2,NULL);
    printf("Counter = %d\n", counter);
    sem_destroy(&sem); 
    return 0;
}





