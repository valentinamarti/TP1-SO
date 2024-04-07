#include "app.h"

int main(int argc, char * argv[]) {
    
    pid_t pid;
    
    int status;

    int children_idx;
    int files_idx;

    
    unsigned int amount_of_files = argc - 1;

    unsigned int amount_to_send;

    unsigned int amount_of_children = get_children_amount(amount_of_files, &amount_to_send);

    pid_t pid_children[amount_of_children];

    int fd_in_children[amount_of_children];
    int fd_out_children[amount_of_children];

    char buff[MAX_RESULT_LENGTH + 1];

    sharedMemoryInfoADT shm = openSharedMemory(getpid(), amount_of_files * (MAX_RESULT_LENGTH + 1), PROT_WRITE);
    size_t final_shm_size = 0;

    printf("%d %d\n", getpid(), amount_of_files * (MAX_RESULT_LENGTH + 1));
    sleep(SLEEP_TIME);

    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {

        int fd_in[PIPE_FILEDESCRIPTORS];
        int fd_out[PIPE_FILEDESCRIPTORS];

        validate(pipe(fd_in), PIPE_ERROR_MSG);

        validate(pipe(fd_out), PIPE_ERROR_MSG);

        pid = fork();
        validate(pid, FORK_ERROR_MSG);
        if(pid == 0) {
            // Child process // read(shm_fd2, shm_buff, MAX_LENGTH);
            char * child_argv[] = {"./childx", NULL};
            char * child_env[] = {NULL};

            // fd_in --> father writes info, child reads
            // fd_out --> child writes info, father reads

            close(fd_in[PIPE_WRITE_END]);   // Child does not write in the father's standard input
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
            fd_in_children[children_idx] = fd_in[PIPE_WRITE_END];      // Passes the info to the child
            fd_out_children[children_idx] = fd_out[PIPE_READ_END];    // Reads the info from the child
            }
    }

    // Modification of the pipes buffering
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        setvbuf(fdopen(fd_in_children[children_idx], "w"), NULL, _IONBF, 0);
        setvbuf(fdopen(fd_out_children[children_idx], "r"), NULL, _IONBF, 0);
    }


    int files_read = 0;
    int files_to_send = amount_of_files;
    files_idx = 1;
    int children_status[amount_of_children];
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        int files_sent = 0;
        children_status[children_idx] = 0;
        while (files_sent < amount_to_send) {
            write(fd_in_children[children_idx], argv[files_idx], strlen(argv[files_idx]));
            write(fd_in_children[children_idx], "\n", 1);
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

                if (children_status[children_idx] == 0) {
                    if (files_to_send == 0) {
                        close(fd_out_children[children_idx]);
                    } else {
                        write(fd_in_children[children_idx], argv[files_idx], strlen(argv[files_idx]));
                        write(fd_in_children[children_idx], "\n", 1);
                        files_idx++;
                        files_to_send--;
                        children_status[children_idx]++;
                    }
                }
            }
        }
    }

    closeSharedMemory(shm);

    // Closing of pipes
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        close(fd_in_children[children_idx]);
    }

    // Waiting for children to finish
    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {
        validate(waitpid(pid_children[children_idx], &status, 0), WAITPID_ERROR_MSG);
//        printf("The child %d has died\n", pid_children[children_idx]); //Despues borrar
    }
    return 0;
}

unsigned int get_children_amount(unsigned int amount_of_files, unsigned int * amount_to_send) {
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
