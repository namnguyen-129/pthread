#include <stdio.h>
#include <stdlib.h>

void modifyPointer(void** ptr) {
    // Thay đổi giá trị mà con trỏ ptr trỏ tới
    int* newValue = malloc(sizeof(int)); // Cấp phát bộ nhớ cho một số nguyên
    *newValue = 42; // Gán giá trị cho số nguyên
    *ptr = newValue; // Gán địa chỉ của newValue cho con trỏ ptr
}

int main() {
    void* retval = NULL; // Một con trỏ void* ban đầu trỏ tới NULL

    printf("Before modifyPointer: retval = %p\n", retval);

    // Truyền địa chỉ của retval (void**) vào hàm modifyPointer
    modifyPointer(&retval); 

    printf("After modifyPointer: retval = %p\n", retval);

    // Ép kiểu void* về int* để sử dụng giá trị
    int* result = (int*)retval;
    printf("Value pointed by retval: %d\n", *result);

    // Giải phóng bộ nhớ được cấp phát
    free(result);

 
    return 0;
}