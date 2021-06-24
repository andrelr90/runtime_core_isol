#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
    int amma = syscall(548, 2462);
    printf("System call returned %d\n", amma);
    return 0;
}