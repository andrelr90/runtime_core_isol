#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int amma = syscall(548, atoi(argv[1]));
    printf("System call returned %d\n", amma);
    return 0;
}
