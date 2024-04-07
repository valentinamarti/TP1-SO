#include "child.h"


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

    char * line = NULL;
    size_t linecap = 0;
    char buff[MAX_SIZE_BUFF];
    char hash_result[MAX_SIZE_BUFF];
    char path_result[MAX_SIZE_BUFF];

    int fd[PIPE_FILEDESCRIPTORS];

    while ((getline(&line, &linecap, stdin)) > 0) {
        validate(pipe(fd), PIPE_ERROR_MSG);
        pid = fork();
        validate(pid, FORK_ERROR_MSG);
        if (pid == 0) {
            // Child process
            char absolute_path[1024 + 1];
            realpath(line, absolute_path);               
            absolute_path[strlen(absolute_path) - 1] = '\0'; // Realpath() copy in "absolute path"; the absolute path of "line" with a \n at the end

            char * md5_argv[] = {"/usr/bin/md5sum", absolute_path, NULL};
            char * md5_env[] = {NULL};

            // Redirect standard error to error.log (when a path is a directory it printed "/.../... Is a directory in the console)
            int error_log_fd = open("error.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            validate(error_log_fd, "ERROR: error when opening error.log");

            if (dup2(error_log_fd, STDERR_FILENO) == -1) {
                perror("dup2");
                close(error_log_fd);
                exit(EXIT_FAILURE);
            }
            close(error_log_fd);

            close(fd[PIPE_READ_END]);
            close(STDOUT);
            dup(fd[PIPE_WRITE_END]);
            close(fd[PIPE_WRITE_END]);
            
            validate(execve("/usr/bin/md5sum", md5_argv, md5_env), EXECVE_ERROR_MSG);
        }

        close(fd[PIPE_WRITE_END]);
        read(fd[PIPE_READ_END], buff, MAX_SIZE_BUFF);
        format_string(buff, hash_result, path_result);

        char resultData[MAX_SIZE_BUFF*3];
        int length = sprintf(resultData, "PID: %d, path: %s, md5 hash: %s \n", getpid(), path_result, hash_result);
        validate((int) write(STDOUT, resultData, length + 1), "ERROR: error when writing in pipe_out");
    }
    free(line);

    while ( (pid = waitpid(-1, &status, 0)) > 0);
}

