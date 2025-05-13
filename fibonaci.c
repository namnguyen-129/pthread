#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdint.h>
int fibonacci(int n) {
    if (n == 0) return 0;
    else if (n == 1 || n ==2) {
        return 1;
    }
    else return fibonacci(n - 1) + fibonacci(n - 2);
}
int64_t fibonaci_eq(int n){
    double a = sqrt(5);
    double phi = (1+a)/2.0;
    double psi = (1-a)/2.0;
    double fib = (pow(phi, n) - pow(psi, n)) / a;// để double rồi mới ép kiểu vì các giá trị tính được toàn là double 
    return (int64_t)fib;
}
int main(){
    int n;
    int fib;
    int64_t fib_eq;
    printf("Nhap n:");
    scanf("%d", &n);
    if (n<=0){
        printf("Error");
    }
    else{
        //fib = fibonacci(n);
        fib_eq = fibonaci_eq(n);
    }
    printf("Fibonacci of %d is %d\n", n, fib);
    printf("Fibonacci of %d is %ld\n", n, fib_eq);
    
}