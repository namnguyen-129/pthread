#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
void* worker(void* arg) {
    printf("Hello from thread %lu\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t tid; // pthread đại diện cho ID của mỗi thread và dượcd lưu vào tid 
    pthread_attr_t attr; // pthread_attr_t là kiểu dữ liệu dùng để lưu trữ các thuộc tính của thread

    // 1. Khởi tạo attr với giá trị mặc định
    pthread_attr_init(&attr);

    // 2. Đặt trạng thái detach ngay khi tạo, tự giải phóng tài nguyên khi kết thúc không cần thread_join
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // 3. Tăng stack size (ví dụ lên 2MB)
    pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);

    // 4. Tạo thread với attr đã thiết lập
    pthread_create(&tid, &attr, worker, NULL);

    // 5. Hủy attr 
    pthread_attr_destroy(&attr);

    
    sleep(1); 

    printf("Main thread kết thúc\n");
    return 0;
}
