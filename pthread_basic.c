#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
// void* thread_func(void* arg) {
//     int id = *(int*)arg;
//     printf("Thread %d is running (TID=%lu)\n", id, pthread_self());
//     return NULL;
// }

// int main() {
//     pthread_t tid1, tid2; //pthread đại diện cho các thread trong cùng tiến trình 
//     // TID được hdh(kernel) gán cho mỗi thread để phân biệt các thread trong cùng tiến trình 
//     // => pthread_self trả về ID của thread hiện tại
    
//     int id1 = 1, id2 = 2;
//     pid_t pid = getpid(); // ID của tiến trình hiện tại
//     printf("Process ID: %d\n", pid); // ID của tiến trình hiện tại
//     // Tạo 2 thread
//     pthread_create(&tid1, NULL, thread_func, &id1);
//     pthread_create(&tid2, NULL, thread_func, &id2);
//     printf("ID of thread 2: %lu\n", pthread_self()); // có thể có lúc in ra cái này trước khi in pthread_create 2 do scheduler
//     // vì lệnh printf này chạy trong main ngay sau khi thread 2 được tạo và có thể lúc đó chưa thực hiện hàm của thread 2 để print 
//     // ra cái trong thread_func và lệnh ID of thread 2 sẽ in ra trước 
//     // nếu tid2 thay thành pthread_self() thì sẽ in ra ID của thread đang thực thi(tức main)
//     // Chờ cả 2 thread kết thúc
//     pthread_join(tid1, NULL);
//     pthread_join(tid2, NULL);

//     printf("Both threads have finished.\n");
//     return 0;
//}

/*các hàm đơn giản của pthread*/
void* worker(void* arg) { // hàm thực hiện của thread, và tham số truyền vào phải là kiểu void*
    int id = *(int*)arg; // ép kiểu và sẽ truyền vào địa chỉ của biến id thì int id sẽ *(đc) tức ra giá trị bên trong ô nhớ đó 
    printf("Thread %d: đang chạy (TID=%lu)\n", id, pthread_self());
    return NULL;
}
void* print(void* args){
    int id = *(int*)args;
    if(id){
        // Phải qua (intptr_t) để đảm bảo kích thước đủ để lưu trữ một con trỏ
        // Nếu là hằng số thì trực tiếp ép (void*)100 vì hằng số không có địa chỉ thực nên không cần thao tác bộ nhớ
        pthread_exit((void*)(intptr_t) id); // Ép kiểu an toàn
    }
    else printf("Pthread id %d\n", id);
}
int main() {
    pthread_t tid1, tid2;
    int id1 = 1, id2 = 5;
     // Tạo 2 thread
    if (pthread_create(&tid1, NULL, worker, &id1) != 0) { //ID của thread được lưu vào tid1; thuộc tính khởi tạo mặc định (attr = NULL)
    // hàm nó thưc hiện là worker; int* sẽ trỏ đến ô nhớ chứa biến id1, và *(int*) tức giải địa chỉ, lấy giá trị trong ô nhớ đó đưa ra
        perror("pthread_create"); exit(1);
    }
    if (pthread_create(&tid2, NULL, print, &id2) != 0) {
        perror("pthread_create"); exit(1);
    }
    void* result;
    void** a = &result;// một con trỏ trỏ đến địa chỉ của con trỏ result kiểu void
    // Đợi 2 thread kết thúc
    pthread_join(tid1, NULL); // pthread_join là hàm chờ cho thread kết thúc 
    pthread_join(tid2, a); // truyền vào địa chỉ của một biến con trỏ kiểu void* để in giá trị vào 
    int kq = (intptr_t) result; // Ép kiểu an toàn từ void* của result về int; result có giá trị của id2 khi đã ép kiểu về void*
    // (intptr_t) là một kiểu nguyên nhưng có kích thước bầng con trỏ và int kq = intptr_t thì sé tự chuyển vì int kích thước nhỏ hơn intptr_t)
    printf("Result of thread is %d\n", kq);

    printf("Finished\n");
    return 0;
}

// Khi muốn trả về con trỏ mảng thì phải cấp phát động và lưu vào vùng heap trành lưu vào stack vì sẽ 
// bị giải phóng khi kết thúc hàm -> dangling pointer -> cash

// #include <pthread.h>
// void* work(void* args){
//     int *heap_arr = malloc(5*sizeof(int));
//     for(int i = 0; i < 5; i++){
//         heap_arr[i] = i + 10;
//     }
//     pthread_exit(heap_arr); // không cần ép kiểu vì int* có thể tự chuyển thành void* để phù hợp
//     // với pthread yêu cầu, nhưng nếu heap_arr là void* mà muốn trả về int* thì phải ép
// }
// int main(){
//     pthread_t tid;
//     pthread_create(&tid, NULL, work, NULL);
//     void* result;
//     pthread_join(tid, &result);
//     int *kq = (int*)result; // ép kiểu từ void* về int*, trong C có thể k ép cx được còn C++ thì phải làm
//     printf("Result is: %d\n", kq[0]);
//     printf("ID of thread is: %lu\n", pthread_self()); //pthread_self() trả về ID của thread hiện tại 
//     free(kq);
//     return 0;
// }

