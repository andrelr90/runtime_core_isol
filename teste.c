#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

//int main(int argc, char *argv[])
//{
//    int amma = syscall(548, atoi(argv[1]));
//    printf("System call returned %d\n", amma);
//    return 0;
//}

int main(int argc, char *argv[])
{
    /*Spawn a child to run the program.*/
    pid_t pid=fork();
    if (pid==0) { /* child process */
	printf("Aqui \n");
        char *argv_aux[]={"firefox", NULL};
        int i = execv(argv[1],argv_aux);
	perror("execv");
	printf("%d, i \n", i);
        exit(127); /* only if execv fails */
    }
    else { /* pid!=0; parent process */
	int amma = syscall(548, pid);
    	printf("System call returned %d\n", amma);
        waitpid(pid,0,0); /* wait for child to exit */	
    }
    return 0;
}

