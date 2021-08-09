#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int restore_masks(int pid_parent){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lcounter = 0, size_file = 0;
    int size_mask_hex, i, counter, integer_mask;

    fp = fopen("/dump_processes", "r");
    if(fp != NULL)
        while ((read = getline(&line, &len, fp)) != -1)
            size_file ++;
    fclose(fp);
	free(line);
    len = 0;
    size_file /= 4;

    int *pids  = (int *)malloc(sizeof(int) * size_file);
    int *masks = (int *)malloc(sizeof(int) * size_file);

    fp = fopen("/dump_processes", "r");
    if (fp != NULL){
        while ((read = getline(&line, &len, fp)) != -1){
            if (lcounter % 4 == 0){
                // pid
                pids[lcounter/4] = atoi(line);
            } else if (lcounter % 4 == 1){
                // mask
                size_mask_hex = read-1;
                // convert hexa to decimal mask.
                integer_mask = 0;
                for (i=size_mask_hex-1, counter=0; i>=0; i--, counter++){
                    if (line[i] >= 97)      // abcde...
                        integer_mask += (line[i] - 87)*pow(16, counter);
                    else if (line[i] >= 65) // ABCDE...
                        integer_mask += (line[i] - 55)*pow(16, counter);
                    else if (line[i] >= 48) // 12345...
                        integer_mask += (line[i] - 48)*pow(16, counter);
                }
                masks[lcounter/4] = integer_mask;
            }
            lcounter ++;
        }
        
        fclose(fp);
        free(line);
    }
    
    long int status = syscall(549, pids, masks, size_file, pid_parent);
    printf("System call restore returned %ld.\n", status);
	
	free(pids);
	free(masks);

    return status;
}

int main(int argc, char *argv[])
{
    if(getuid() != 0){
        printf("Must be executed with sudo.\n");
        return -1;
    }

    printf("Dumping running processes.\n");
    system("bash get_running_processes > /dump_processes 2> /dev/null");
    printf("Dump concluded.\n");
    int i, aux, isol;

	char *argv1_copy = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
	char *token = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
	char *program_name = (char*)malloc((strlen(argv[1])+1)* sizeof(char));
	char **argv_aux = (char**)malloc((argc)* sizeof(char *)); /* Argv to new process */
	
	for(i = 0; i < argc; i++){
		argv_aux[i] = NULL;
	}

    /*Spawn a child to run the program.*/
    pid_t pid=fork();
    if (pid==0) { /* child process */
		strcpy(argv1_copy, argv[1]); /* copy argv, because strtok changes the string */
		token = strtok(argv1_copy, "/");

		while(token != NULL) { /* Loop to get the program name to argv[0] */
			program_name = (char *)realloc(program_name, (strlen(token)+1)* sizeof(char));
			strcpy(program_name, token);
			token = strtok(NULL, "/");
		}

		argv_aux[0] = (char*)malloc((strlen(program_name)+1) * sizeof(char));
		strcpy(argv_aux[0], program_name);

		if(argc >= 3){ /* Getting the argv off new process */
			aux = 1;
			for(i = 2; i < argc; i++){
				argv_aux[aux] = (char*)malloc((strlen(argv[i])+1) * sizeof(char));
				strcpy(argv_aux[aux], argv[i]);
				aux ++;
			}
		} else {
        	argv_aux[1] = NULL;
		}

        i = execv(argv[1],argv_aux);
        perror("execv");
        printf("%d, i \n", i);
       	exit(127); /* only if execv fails */
    }
    else { /* pid!=0; parent process */
        pid_t parent_pid = getpid();
        printf("Parent pid: %d\n", parent_pid);
        printf("Child pid: %d\n", pid);

        isol = syscall(548, pid);
       	printf("System call isolate returned %d\n", isol);
        waitpid(pid,0,0); /* wait for child to exit */

        printf("Restoring masks.\n");
        restore_masks(parent_pid);
    }
	free(token);
	free(program_name);
	free(argv1_copy);
	for(i = 0; i < argc-1; i++){
		free(argv_aux[i]);
	}
	free(argv_aux);
    return 0;
}
