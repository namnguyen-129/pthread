/*        Cách đơn giản nhất nhưng có thể xẩy ra cả deadlock và cả starvation   */
// #include <stdio.h>
// #include <pthread.h>
// #include <unistd.h>

// #define N 5

// pthread_mutex_t chopstick[N];

// void* philosopher(void* num) {
//     int id = *(int*)num;

//     while (1) {
//         printf("Philosopher %d is thinking\n", id);
//         sleep(1);

        
//         pthread_mutex_lock(&chopstick[id]);             
//         pthread_mutex_lock(&chopstick[(id+1)%N]);       

//         printf("Philosopher %d is eating\n", id);
//         sleep(2);

//         // Put down chopsticks
//         pthread_mutex_unlock(&chopstick[id]);
//         pthread_mutex_unlock(&chopstick[(id+1)%N]);
//     }
// }

// int main() {
//     pthread_t tid[N];
//     int id[N];

//     for (int i = 0; i < N; i++)
//         pthread_mutex_init(&chopstick[i], NULL);

//     for (int i = 0; i < N; i++) {
//         id[i] = i;
//         pthread_create(&tid[i], NULL, philosopher, &id[i]);
//     }

//     for (int i = 0; i < N; i++)
//         pthread_join(tid[i], NULL);

//     return 0;
// }
/*                Có một người không ngồi vào bàn => k bị deadlock            */
/* các triết gia đầu tiên xem running > 0 không nếu có 
các triết giá suy nghĩ và chỉ 4 ngươi được vào phòng (đến khi room = 0) người còn lại phải đợi khi 
có ai post room, triết gia sẽ cầm đũa trái và phải lên(nếu đã có người cầm rỏi thì phải đợi ai post 
đũa trái or phải) và ăn, sau đó đánh dấu mình đã ăn, kiểm tra xem all ăn chưa, nếu tất cả đã ăn rồi thì post room 
đế các triết gia đang đợi sẽ vào phòng để tiếp tục cầm đũa ăn sau đó lại đi ra(vif vậy sau khi in mọi 
người đều ăn rổi thì vẫn in thêm người 1, 2 ăn chẳng hạn) và lúc này mọi ngươi mới
định vào đều phải xem running có = 0 không nếu = 0 thì dừng k ai vào nữa*/
// #include <stdio.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h>
// #define Num 5
// sem_t chopstick[Num]; // mỗi đũa đều là mutex
// sem_t room;
// int running = 1;
// int eaten[Num] = {0}; // mảng này để kiểm tra xem ai ăn rồi
// pthread_mutex_t done;
// int eaten_count = 0;
// void* philosopher(void* num){
//     int id = *(int*)num;
//     while(running){
//         printf("Philosopher %d is thinking\n", id); // nghixem có vào phòng không
//         sleep(1);
//         // lần lượt từng người vào đến 4 người thôi là về 0 k ai vào nữa
//         sem_wait(&room);
        
//         sem_wait(&chopstick[id]); // lấy đũa trái
//         sem_wait(&chopstick[(id+1) % Num]); // lấy đũa phải

//         printf("Philosopher %d is eating\n", id); // ăn
//         sleep(2);

//         pthread_mutex_lock(&done);
//         // Kiểm tra xem mọi người ăn hết chưa
//         if(eaten[id] == 0){
//             eaten[id] = 1;
//             eaten_count ++; // số người đã ăn tăng lên 1
//             printf("Philosopher %d has eaten\n", id);
//         }
//         if (eaten_count == Num){
//             running = 0; // nếu tất cả mọi người đã ăn thì dừng lại
//             printf("All philosophers have eaten\n"); 
//             for (int k = 0; k < Num; k++) { 
//                 sem_post(&room);
//             }
//         }
//         pthread_mutex_unlock(&done);
//         sem_post(&chopstick[id]);  // buông đũa
//         sem_post(&chopstick[(id+1) % Num]); // buông đũa
//         sem_post(&room); // ra khỏi phòng 
//     }
// }
// int main(){
//     pthread_t tid[Num];
//     pthread_mutex_init(&done, NULL);
//     int id[Num];
//     sem_init(&room, 0, 4);
//     for(int i = 0; i < Num; i ++){
//         sem_init(&chopstick[i], 0, 1);
//         id[i] = i;
//     }
//     for(int i = 0; i < Num; i ++){
//         pthread_create(&tid[i], NULL, philosopher, &id[i]);
//     }
//     for(int i = 0; i < Num; i ++){
//         pthread_join(tid[i], NULL); // để như này thì sẽ mãi không kết thúc vì while(1) 
        
//     }
//     sem_destroy(&room);
//     for(int i = 0; i < Num; i ++){
//         sem_destroy(&chopstick[i]);
//     }
//     pthread_mutex_destroy(&done); // hủy mutex
// }



