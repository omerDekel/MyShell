#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

# define COMMAND_LENGTH 124
# define ARGS_SIZE 10
# define JOBS_SIZE 40

typedef struct job {
    pid_t pid;
    char jobName[COMMAND_LENGTH];
} job;

void InitializeJobs(job jobs[JOBS_SIZE]) {
    for (int i = 0; i < JOBS_SIZE ; ++i) {
        strcpy(jobs[i].jobName, "Available");
    }
}
int AddJob(job newJob, job jobs[JOBS_SIZE]) {
    int stat;
    for (int i = 0; i <JOBS_SIZE ; ++i) {
        if(!strcmp(jobs[i].jobName,"Available") || waitpid(jobs[i].pid ,&stat , WNOHANG)) {
            jobs[i] = newJob;
            return 1;
        }
    }
    return 0;
}
void printJobs (job jobs[JOBS_SIZE]) {
    int stat;
    for (int i = 0; i < JOBS_SIZE ; ++i) {
        if(strcmp(jobs[i].jobName,"Available") && !waitpid(jobs[i].pid , &stat , WNOHANG)) {
            printf("%d %s \n",jobs[i].pid, jobs[i].jobName);
        }
    }
}
int getArgs(char *args[ARGS_SIZE],char cmd[COMMAND_LENGTH]) {
    int i = 0;
    char *arg = strtok(cmd , " ");
    while (arg != NULL) {
        args[i++] = arg;
        //printf("argum %d is ", i);
        //printf("%s",args[i]);
        arg = strtok(NULL , " ");
    }
    if (!strcmp(args[i - 1] , "&")) {
        args[i-1] = NULL;
        return 0;
    }
    args[i] = NULL;
    return 1;
}
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
        if (chdir(args[1]) == -1) {
            printf(" no such directory\n");
            return -1;
        }
        return 0;
    }
}

int main() {
    int wait, ret_code ,stat;
    pid_t pid;
    char *args[ARGS_SIZE];
    char cmd[COMMAND_LENGTH];
    char cmdCopy[COMMAND_LENGTH];
    job jobsArr[JOBS_SIZE];
    InitializeJobs(jobsArr);
    while (1) {
        printf("prompt >");
        fgets(cmd, COMMAND_LENGTH * sizeof(char), stdin);
        if (cmd[0]=='\n') {
            continue;
        }
        if (strlen(cmd) > 0) {
            cmd[strlen(cmd) - 1] = '\0';
        }
        strcpy(cmdCopy , cmd);
        if (!strncmp(cmd , "cd", 2)) {
            CD(args);
        } else if (!strcmp(cmdCopy, "jobs")) {
            printJobs(jobsArr);
        } else {
            wait = getArgs(args,cmd);
            pid = fork();
            if (pid < 0) {
                printf("error in fork\n");
                return -1;
            }
            if (pid == 0) {
                /*son*/
                ret_code = execvp(args[0] , args);
                fprintf(stderr , "error in system call\n");
                if (ret_code == -1) {
                    exit(-1);
                }
            } else {
                /*father*/
                printf(" pid :%d\n" , pid);
                if (wait) {
                    waitpid(pid , &stat , 0);
               } else {
                   job job1;
                   job1.pid = pid;
                    cmdCopy[strlen(cmdCopy)-1] = '\0';
                   strcpy(job1.jobName , cmdCopy);
                   if (!AddJob(job1,jobsArr)) {
                       printf("could not add the job to the array");
                   }
               }
            }
        }
    }

    return 0;
}