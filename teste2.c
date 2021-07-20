#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

void print_adresses_values(int* arr, int n){
    for(unsigned i = 0; i < n; i++){
        printf("end = %p \t value = %d\n", arr+i, *(arr+i));
    }
    printf("\n");
}

int main(int argc, char* argv[]){
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
    len = 0;
    size_file /= 4;
    printf("file size: %d\n", size_file);

    int *pids  = malloc(sizeof(int) * size_file);
    int *masks = malloc(sizeof(int) * size_file);

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
        if (line)
            free(line);
    }

    for(i=0; i<size_file; i++){
        printf("%d: %d\n", pids[i],masks[i]);
    }
    
    long int amma = syscall(549, pids, masks, size_file);
    printf("System call sys_hello returned %ld\n", amma);

    return 0;
}