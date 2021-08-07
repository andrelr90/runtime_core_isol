#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

void foo(int* arr, int n){
    /*
    printf("func = %ld %ld\n", sizeof(arr), sizeof(arr[0]));
    printf("end func= %p\n", arr);
    */
    
    for(unsigned i = 0; i < n; i++){
        printf("end = %p \t value = %d\n", arr+i, *(arr+i));
    }
    printf("\n");
}

void main(int argc, char* argv[]){
    int arr[] = {2320 ,1, 2, 3, 4, 5, 6, 7};
    int size = sizeof(arr)/sizeof(arr[0]);
    /*
    printf("%ld %ld\n", sizeof(arr), sizeof(arr[0]));
    printf("end = %p\n", &arr);
    */

    foo(arr, size);
    long int amma = syscall(549, arr, size);
    printf("System call sys_hello returned %ld\n", amma);
}
