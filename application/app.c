#include "app.h"

int main(int argc, char * argv[]) {
    
    pid_t pid;
    
    int status;

    int children_idx;
    int files_idx;

    unsigned int amount_of_children = get_children_amount(argc-1);

    pid_t pid_children[amount_of_children];

    int fd_in_children[amount_of_children];
    int fd_out_children[amount_of_children];


    int sh_memory = shm_open("sh_memory", O_CREAT | O_RDWR, PROT_WRITE);
    validate(sh_memory, "ERROR: error when creating shared memory");

    //We get the size of the shared memory
    struct stat shm_stat;
    if (fstat(sh_memory, &shm_stat) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    size_t shm_size = shm_stat.st_size;

   printf("El tamaño es: %zu\n", shm_size);


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

            // close(STDOUT);
            // dup(fd_in[PIPE_READ_END]);
            // close(fd_in[PIPE_READ_END]);

            // close(STDIN);
            // dup(fd_out[PIPE_WRITE_END]);
            // close(fd_out[PIPE_WRITE_END]);

            pid_children[children_idx] = pid;
            fd_in_children[children_idx] = fd_in[PIPE_WRITE_END];      // Passes the info to the child
            fd_out_children[children_idx] = fd_out[PIPE_READ_END];    // Reads the info from the child
            }
    }

    // Modification of the pipes buffering
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        setvbuf(fdopen(fd_in_children[children_idx % amount_of_children], "w"), NULL, _IONBF, 0);
        setvbuf(fdopen(fd_out_children[children_idx % amount_of_children], "r"), NULL, _IONBF, 0);
    }

    // Distribution of archives
    for (files_idx = 1, children_idx = 0; files_idx < argc; files_idx++, children_idx++) {
        write(fd_in_children[children_idx % amount_of_children], argv[files_idx], strlen(argv[files_idx]));
        write(fd_in_children[children_idx % amount_of_children], "\n", 1);
    }

    char buff[MAX_LENGTH + 1]; // Just in case, we do MAX_LENGTH + 1
    int k;
    for(k = 0; k < amount_of_children; k++){
        ssize_t bytes_read = read(fd_out_children[k], buff, MAX_LENGTH);

        validate((int) bytes_read, "ERROR: error when reading");
        buff[bytes_read] = '\0';    // Null terminated the string in buffer
        printf("%s", buff);

    }

    // Closing of pipes
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        close(fd_in_children[children_idx]);
        close(fd_out_children[children_idx]);
    }

    // Waiting for children to finish
    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {
        validate(waitpid(pid_children[children_idx], &status, 0), WAITPID_ERROR_MSG);
        printf("The child %d has died\n", pid_children[children_idx]); //Despues borrar
    }
    return 0;
}

int get_children_amount(int amount_of_files) {

    if(amount_of_files > INFLEX_POINT) {
        return (int)(amount_of_files)*0.05;
    }
    if( amount_of_files < MIN_CHILDREN ) {
        return amount_of_files;
    }
    return MIN_CHILDREN;
}
