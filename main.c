#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

# define COMMAND_LENGTH 124
# define ARGS_SIZE 10

/**
 * from stackoverflow
 * @param args
 * @return
 */
int CD(char *args[]) {
    // If we write no path , then go the home directory
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
        // Else we change the directory to the one specified by the argument
    } else {
        if (chdir(args[1] == -1)) {
            printf(" no such directory\n")
            return -1;
        }
        return 0;
    }
}

int main() {
    int wait = 1;
    int stat , ret_code;
    pid_t pid;
    char *args[ARGS_SIZE];
    char cmd[COMMAND_LENGTH];
    char cmdCoppy[COMMAND_LENGTH];
    while (1) {
        printf("prompt >");
        //scanf("%s" , cmd);
        fgets(cmd, COMMAND_LENGTH * sizeof(char), stdin);
        cmd[strlen(cmd) - 1] = '\0';
        strcpy(cmdCoppy , cmd);
        if (!strncmp(cmd , "cd", 2)) {
            CD(args);
        } else {
            int i = 0;
            char *arg = strtok(cmd , " ");
            while (arg != NULL) {
                args[i++] = arg;
                //printf("argum %d is ", i);
                //printf("%s",args[i]);
                arg = strtok(NULL , " ");
            }
            args[i] = NULL;
            pid = fork();
            if (pid < 0) {
                printf("error in fork\n");
                return -1;
            }
            if (pid == 0) {
                /*son*/
                /*int j = 0;
                while (args[j] !=NULL) {
                    printf("argum %d is ", j);
                    printf("%s",args[j]);
                    j++;
                }*/
                //printf(" pid :%d\n" , pid);
                ret_code = execvp(args[0] , args);
                fprintf(stderr , "error in system call\n");
                if (ret_code == -1) {
                    exit(-1);
                }
                //}
            } else {
                /*father*/
               if (strcmp(args[i - 1] , "&")) {
                    printf(" pid :%d\n" , pid);
                    waitpid(pid , NULL , 0);
               }
            }
        }
    }

    return 0;
}