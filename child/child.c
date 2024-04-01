#include "child.h"

char flag = 1;

int main(int argc, char * argv[]) {
    read_and_execute();
    return 0;
}

void format_string(char * md5_result, char * hash_result, char * path_result) {
    char *token;

    token = strtok(md5_result, "  ");
    strcpy(hash_result, token);

    token = strtok(NULL, "  ");
    strcpy(path_result, strtok(token, "\n"));
}


void read_and_execute() {
    pid_t pid;
    int status = 0;
    pid_t wpid;

    signed long linelen;
    char * line = NULL;
    size_t linecap = 0;
    char buff[MAX_SIZE_BUFF];
    char hash_result[MAX_SIZE_BUFF];
    char path_result[MAX_SIZE_BUFF];

    int fd[PIPE_FILEDESCRIPTORS];

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        validate(pipe(fd), PIPE_ERROR_MSG);
        pid = fork();
        validate(pid, FORK_ERROR_MSG);
        if (pid == 0) { // Child process
            char absolute_path[1024 + 1];
            realpath(line, absolute_path);               
            absolute_path[strlen(absolute_path) - 1] = '\0'; // realpath() copy in "absolute path", the absolute path of "line" with a \n at the end

            char * md5_argv[] = {"/usr/bin/md5sum", absolute_path, NULL};
            char * md5_env[] = {NULL};

            close(fd[PIPE_READ_END]);
            close(STDOUT);
            dup(fd[PIPE_WRITE_END]);
            close(fd[PIPE_WRITE_END]);
            
            validate(execve("/usr/bin/md5sum", md5_argv, md5_env), EXECVE_ERROR_MSG);
        }
        close(fd[PIPE_WRITE_END]);
        read(fd[PIPE_READ_END], buff, MAX_SIZE_BUFF);
        format_string(buff, hash_result, path_result);
        fprintf(stdout, "PID: %d, path: %s, md5 hash: %s \n", getpid(), path_result, hash_result);
    }
    while ( (wpid = waitpid(-1, &status, 0)) > 0);
    return;
}

