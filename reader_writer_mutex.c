
#include<stdio.h>
#include<unistd.h>
#include<pthread.h> 
#define Num_reader 4
#define Num_writer 2
#define N_iters 3 
int shared_data = 5;
int read_count = 0;
/*                   Chỉ dùng mutex sẽ ưu tiên hoàn toàn cho  read  => starving writer
chạy hết reader mới sang chạy hết writer       */
// pthread_mutex_t re_mutex; // bảo vệ read_count
// pthread_mutex_t shared_mutex; // shared_data

// void* reader(void* args){
//     int id = *(int*) args;
//     for(int i = 0; i < N_iters; i++){
//         pthread_mutex_lock(&re_mutex);
//         read_count++;
//         if(read_count == 1){
//             pthread_mutex_lock(&shared_mutex); // nếu là reader đầu thì block writer(nếu wr đang unlock thì lock lại)
//         }
//         pthread_mutex_unlock(&re_mutex);
//         /*       Đây là vùng CS mà các reader được phép truy cập            */
//         printf("Reader %d read data: %d\n", id, shared_data);
//         usleep(100);
//         /*    Reader rời đi cũng phải lock  */
//         pthread_mutex_lock(&re_mutex);
//         read_count--;
        
//         if(read_count == 0){
//             pthread_mutex_unlock(&shared_mutex); 
//             // nếu là reader cuối thì mới cho phép writer vào => xẩy ra starving writer
//         }
//         pthread_mutex_unlock(&re_mutex); 
//         usleep(100);
//         // Nếu không có usleep sẽ chỉ có read thôi :))

//     }
// }
// void* writer(void* args){
//     int id = *(int*) args;
//     for(int i = 0; i < N_iters; i ++){
//         pthread_mutex_lock(&shared_mutex);
//     /* Writer truy cập và ghi giá trị mới */
//         shared_data = id * 10 + i;
//         printf("Writer %d write data: %d\n", id, shared_data);
//         pthread_mutex_unlock(&shared_mutex);
//     }
//     usleep(50);
// }
// int main(){
//     pthread_mutex_init(&re_mutex, NULL);
//     pthread_mutex_init(&shared_mutex, NULL);
//     pthread_t readerid[Num_reader], writerid[Num_writer];
//     int rid[Num_reader];
//     int wid[Num_writer];
//     for(int i = 0; i < Num_reader; i ++){
//         rid[i] = i; // Khởi tạo giá trị cho mảng rid
//         pthread_create(&readerid[i], NULL, reader, &rid[i]);
//     }
//     for(int i = 0; i < Num_writer; i ++){
//         wid[i] = i; // Khởi tạo giá trị cho mảng wid
//         pthread_create(&writerid[i], NULL, writer, &wid[i]);
//     }
//     for(int i = 0; i < Num_reader; i ++){
//         pthread_join(readerid[i], NULL);
//     }
//     for(int i = 0; i < Num_writer; i ++){
//         pthread_join(writerid[i], NULL);
//     }
//     pthread_mutex_destroy(&re_mutex);
//     pthread_mutex_destroy(&shared_mutex);
// }

/*                   Kết hợp conđition var để writer không bị starving           */
pthread_mutex_t rw_mutex; // bảo vệ read_count, writer_count
pthread_mutex_t shared_mutex; // shared_data
pthread_cond_t wr_queue; // hàng đợi cho writer
pthread_cond_t re_queue; // hàng đợi cho reader
int writer_count = 0; // tránh việc chỉ đọc không có ghi
void* reader(void* args){
    int id = *(int*) args;
    for(int i = 0; i < N_iters; i++){
        pthread_mutex_lock(&rw_mutex);
        
        while(writer_count > 0){
            pthread_cond_wait(&re_queue, &rw_mutex); 
    // nếu có writer thì phải ưu tiên, nên reader cho vào hang đợi
        }
        read_count++;
        if(read_count == 1){ // nếu reader đầu thì block vùng shared_data
            pthread_mutex_lock(&shared_mutex); 
    // nếu là reader đầu thì block writer(nếu wr đang unlock thì lock lại)
        }
// Reader đầu tiên đã lock vùng shared-data rồi nên writer không thể chen vào mà ghi được nên phải đợi
// các reader sau thì vẫn cứ đọc bình thường vì lock rồi nênn k cần lo nữa
        pthread_mutex_unlock(&rw_mutex); // unlock vì lúc này không cần thay đổi gì read_count cả 

        printf("Reader %d read data: %d\n", id, shared_data);
        usleep(10000);
        /*      Reader rời đi       */
        pthread_mutex_lock(&rw_mutex);// lock vào để thay đổi read_count
        read_count--;
        if(read_count == 0){
            pthread_mutex_unlock(&shared_mutex); // mở ra cho writer được vào ghi 
        }
        pthread_mutex_unlock(&rw_mutex); 
        usleep(10000);
    }
}
void* writer(void* args){
    int id = *(int*) args;
    for(int i = 0; i < 10; i++){ // 
        pthread_mutex_lock(&rw_mutex);
        writer_count++;
        pthread_mutex_unlock(&rw_mutex); 
// unlock nếu k sẽ deadlock vì reader đầu tiên đang giữ shared_mutex,sau mỗi lần đọc thì lại giải phóng
// rw_mutex cho các reader khcs đọc nhưng nếu reader vưà thả rw_mutex thì writer lại đi đến
// và lock rw_mutex và tăng writer_count lên 1 thì nó sẽ giữ rw_mutex, nếu không giải phóng 
// thì reader tiếp theo sẽ không thể lock rw_mutex ở ngay sau chỗ for í được để vào 
// kiểm tra writer_count > 0 nên sẽ bị deadlock chứ nếu nó vào được và nó thấy có writer chờ nó sẽ cho 
// writer thực hiện trước 
        pthread_mutex_lock(&shared_mutex); // <---- deadlock ở đây vì 


        /*           Writer truy cập để ghi       */
        shared_data = id * 10 + i;
        printf("writer %d write :%d\n", id, shared_data);
        usleep(500);


        pthread_mutex_unlock(&shared_mutex);
        //pthread_mutex_unlock(&rw_mutex);
        /*            Writer out            */
        pthread_mutex_lock(&rw_mutex);
        writer_count--;
        if(writer_count == 0){ 
            pthread_cond_broadcast(&re_queue); // đánh thức toàn bộ reader trong hàng đợi vì reader có thể vào đọc cùng nhau
        }
        pthread_mutex_unlock(&rw_mutex);
        usleep(5000);
    }
}
int main(){
    pthread_mutex_init(&rw_mutex, NULL);
    pthread_mutex_init(&shared_mutex, NULL);
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
    pthread_mutex_destroy(&rw_mutex);
    pthread_mutex_destroy(&shared_mutex);
}



/*              Dùng mutex + condition               */

// pthread_mutex_t mutex; // để bảo vệ biến đếm là wr và read_count
// pthread_mutex_t wr;   // để bảo vệ vùng CS (shared data) của writer
// pthread_cond_t wr_queue;
// pthread_cond_t re_queue; // hàng đợi cho reader
// int wr_waiting = 0; // tránh việc chỉ đọc không có ghi
// void* reader(void* args){
//     int id = *(int*) args;
//     for(int i = 0; i < N_iters; i ++){
//         pthread_mutex_lock(&mutex);
//         while(wr_waiting > 0){
//             pthread_cond_wait(&wr_queue, &mutex);
//         }
//         read_count++;
//         // while(0< read_count < Num_reader){
//         //     pthread_cond_wait(&enable_read, &mutex); 
//         //     // nếu có reader thì writer không được vào và phải đợi trong hàng đợi 
//         //     // khi một thread khác gọi pthread_cond_signal(&enable_read) thì thread này sẽ được đánh thức 
//         // }
//         if(read_count == 1){
//             pthread_mutex_lock(&wr); 
// // nếu là reader đầu thì block writer(nếu wr đang unlock thì lock lại)
//         }
//         pthread_mutex_unlock(&mutex); 
        
//         /*       Các reader khác có thể truy cập vùng CS vì đã mở mutex rồi       */
//         printf("Reader %d read: %d\n", id, shared_data);
//         usleep(10);

//         /*        Reader rời đi, phải khóa mutex vì truy cập read_count              */
//         pthread_mutex_lock(&mutex);
//         read_count --;
//         if(read_count == 0){
//             // pthread
//             pthread_cond_signal(&wr_queue); // đánh thức writer trong hàng đợi 
//         }
//         pthread_mutex_unlock(&mutex);
//     }
// }
// void* writer(void* args){
//     int id =*(int*) args;
    
//     for(int i = 0; i < N_iters; i ++){
//         pthread_mutex_lock(&mutex); // khóa mutex lại 
//         wr_waiting++;
//         pthread_mutex_unlock(&mutex); // writer truy cập để ghi 
//         pthread_mutex_lock(&wr);

//         /*       Đây là vùng CS mà writer được phép truy cập để ghi            */
//         shared_data = id * 10 + i;
//         printf("Writer %d write: %d\n", id, shared_data);
//         pthread_mutex_unlock(&wr); // mở mutex để cho phép writer khác vào

//         /*        Writer rời đi          */


//         pthread_mutex_lock(&mutex);
//         wr_waiting--;
//         pthread_cond_broadcast(&re_queue); // đánh thức tất cả reader trong hàng đợi
//     }
// }
// int main(){
//     pthread_mutex_init(&mutex, NULL);
//     pthread_mutex_init(&wr, NULL);
//     pthread_cond_init(&wr_queue, NULL);
//     pthread_cond_init(&re_queue, NULL);

//     pthread_t readerid[Num_reader], writerid[Num_writer];
//     int rid[Num_reader];
//     int wid[Num_writer];

//     for (int i = 0; i < Num_reader; i++) {
//         rid[i] = i;
//         pthread_create(&readerid[i], NULL, reader, &rid[i]);
//     }
//     for (int i = 0; i < Num_writer; i++) {
//         wid[i] = i;
//         pthread_create(&writerid[i], NULL, writer, &wid[i]);
//     }

//     for (int i = 0; i < Num_reader; i++) {
//         pthread_join(readerid[i], NULL);
//     }
//     for (int i = 0; i < Num_writer; i++) {
//         pthread_join(writerid[i], NULL);
//     }

//     pthread_mutex_destroy(&mutex);
//     pthread_mutex_destroy(&wr);
//     pthread_cond_destroy(&wr_queue);
//     pthread_cond_destroy(&re_queue);
// }