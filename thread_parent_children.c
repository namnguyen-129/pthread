#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h> // Thêm thư viện này để sử dụng wait()

int main(){
    pid_t pid = fork();
    if (pid > 0) { 
        wait(NULL); // Đợi tiến trình con kết thúc
        printf("Parent process: Created child with ID = %d\n", pid);
        
    } else if (pid == 0){ 
        printf("Child process: Parent ID = %d, My ID = %d\n", getppid(), getpid());
    } else {
        perror("Fork failed");
    }
}
