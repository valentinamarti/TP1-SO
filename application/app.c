#include "app.h"

int main(int argc, char * argv[]) {
    
    pid_t pid;
    
    int status;

    int children_idx;
    int files_idx;

    unsigned int amount_of_files = argc - 1;

    char ** files = getOnlyFilesPaths(argv, argc, &amount_of_files);

    if(amount_of_files == 0){
        perror("ERROR: program cannot run; no files where provided.");
        exit(errno);
    }

    FILE * shm_content;
    
    if((shm_content = fopen("./shm_content.txt", "w")) == NULL){
        perror(OPEN_FILE_ERROR_MSG);
        exit(EXIT_FAILURE);
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    unsigned int amount_to_send;

    unsigned int amount_of_children = getChildrenAmount(amount_of_files, &amount_to_send);

    pid_t pid_children[amount_of_children];

    int fd_in_children[amount_of_children];
    int fd_out_children[amount_of_children];

    char buff[MAX_RESULT_LENGTH + 1];

    sharedMemoryInfoADT shm = openSharedMemory(getpid(), amount_of_files * (MAX_RESULT_LENGTH + 100), PROT_WRITE);
    size_t final_shm_size = 0;

    printf("%d %d\n", getpid(), amount_of_files * (MAX_RESULT_LENGTH + 100));
    sleep(SLEEP_TIME);

    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {

        int fd_in[PIPE_FILEDESCRIPTORS];
        int fd_out[PIPE_FILEDESCRIPTORS];

        validate(pipe(fd_in), PIPE_ERROR_MSG);

        validate(pipe(fd_out), PIPE_ERROR_MSG);

        pid = fork();
        validate(pid, FORK_ERROR_MSG);
        if(pid == 0) {
            // Child process
            char * child_argv[] = {"./childx", NULL};
            char * child_env[] = {NULL};


            close(fd_in[PIPE_WRITE_END]);
            close(STDIN);
            dup(fd_in[PIPE_READ_END]);
            close(fd_in[PIPE_READ_END]);


            close(fd_out[PIPE_READ_END]);
            close(STDOUT);
            dup(fd_out[PIPE_WRITE_END]);
            close(fd_out[PIPE_WRITE_END]);

            validate(execve("./childx", child_argv, child_env), EXECVE_ERROR_MSG);
        } else {
            // Parent process
            close(fd_in[PIPE_READ_END]);
            close(fd_out[PIPE_WRITE_END]);

            pid_children[children_idx] = pid;
            fd_in_children[children_idx] = fd_in[PIPE_WRITE_END];      // Pipe to send info to the child
            fd_out_children[children_idx] = fd_out[PIPE_READ_END];    // Pipe to read info from the child
        }
    }

    // Modification of the pipes buffering
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        setvbuf(fdopen(fd_in_children[children_idx], "w"), NULL, _IONBF, 0);
        setvbuf(fdopen(fd_out_children[children_idx], "r"), NULL, _IONBF, 0);
    }


    int files_read = 0;
    int files_to_send = amount_of_files;
    files_idx = 0;
    int children_status[amount_of_children];
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        int files_sent = 0;
        children_status[children_idx] = 0;
        while (files_sent < amount_to_send) {
            writeInPipe(fd_in_children[children_idx], files[files_idx]);
            files_sent++;
            files_to_send--;
            files_idx++;
            children_status[children_idx]++;
        }
    }


    while (files_read < amount_of_files) {
        fd_set fd_to_read;
        FD_ZERO(&fd_to_read);
        for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
            if (children_status[children_idx] != 0) {
                FD_SET(fd_out_children[children_idx], &fd_to_read);
            }
        }
        validate(select(fd_out_children[amount_of_children - 1] + 1, &fd_to_read, NULL, NULL, NULL), SELECT_ERROR_MSG);

        for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
            if (FD_ISSET(fd_out_children[children_idx], &fd_to_read)) {
                ssize_t bytes_read = read(fd_out_children[children_idx], buff, MAX_RESULT_LENGTH);
                validate((int) bytes_read, READ_ERROR_MSG);
                children_status[children_idx]--;
                files_read++;

                final_shm_size += writeOnSharedMemory(shm, buff);
                fprintf(shm_content, "%s\n", buff);

                if (children_status[children_idx] == 0) {
                    if (files_to_send == 0) {
                        close(fd_out_children[children_idx]);
                    } else {
                        writeInPipe(fd_in_children[children_idx], files[files_idx]);
                        files_idx++;
                        files_to_send--;
                        children_status[children_idx]++;
                    }
                }
            }
        }
    }
    postSem(shm);

    fclose(shm_content);

    freeOnlyFilesPaths(files, amount_of_files);

    //closeSharedMemory(shm);

    // Closing of pipes
    closePipes(fd_in_children, amount_of_children);

    // Waiting for children to finish
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        validate(waitpid(pid_children[children_idx], &status, 0), WAITPID_ERROR_MSG);
    }
    return 0;
}

unsigned int getChildrenAmount(unsigned int amount_of_files, unsigned int * amount_to_send) {
    int amount_of_children;
    if(amount_of_files > INFLEX_POINT) {
        amount_of_children = (unsigned int)(amount_of_files)*0.1;
        *amount_to_send = amount_of_files / AMOUNT_OF_FILES_DISTRIBUTION;
        return amount_of_children;
    }
    if( amount_of_files < MIN_CHILDREN ) {
        *amount_to_send = MIN_DISTRIBUTION;
        return amount_of_files;
    }
    *amount_to_send = MIN_DISTRIBUTION + 2 * (amount_of_files - MIN_CHILDREN) / (INFLEX_POINT - MIN_DISTRIBUTION - MIN_CHILDREN);
    return MIN_CHILDREN;
}

void closePipes(int fds[], unsigned int amount_of_children) {
    for (int children_idx = 0; children_idx < amount_of_children; children_idx++) {
        close(fds[children_idx]);
    }
}

void writeInPipe(int fd, char * buff) {
    write(fd, buff, strlen(buff));
    write(fd, NEWLINE, sizeof(char));
}

char ** getOnlyFilesPaths(char * argv[], int argc, unsigned int * amount_of_files) {
    char ** files = malloc(sizeof(char *) * INITIAL_FILES_ARR_DIM);
    size_t files_size = INITIAL_FILES_ARR_DIM;

    int j = 0;
    for (int i = 1; i < argc; i++) {
        struct stat sb;
        validate(stat(argv[i], &sb), STAT_ERROR_MSG);
        if (S_ISREG(sb.st_mode)) {
            if (j >= files_size) {
                files = realloc(files, sizeof(char *) * (j + INITIAL_FILES_ARR_DIM));
            }
            char * new_file = malloc(sizeof(char) * (strlen(argv[i]) + 1));
            strcpy(new_file, argv[i]);
            files[j++] = new_file;
        }
    }
    files = realloc(files, sizeof(char *) * j);
    *amount_of_files = j;
    return files;
}

void freeOnlyFilesPaths(char ** files, unsigned int amount_of_files) {
    for (int i = 0; i < amount_of_files; i++) {
        free(files[i]);
    }
    free(files);
}
