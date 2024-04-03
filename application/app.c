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


    int sh_memory = shm_open("sh_memory", O_CREAT | O_RDWR);
    validate(sh_memory, "ERROR: error when creating shared memory");

    //Conseguimos el tamaño de la shared memory
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

            // fd_in --> padre ingresa info, hijo lee
            // fd_out --> hijo ingresa info, padre lee

            close(fd_in[PIPE_WRITE_END]);   //hijo no escribe en la entrada del padre
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
            fd_in_children[children_idx] = fd_in[PIPE_WRITE_END];      //pasamos info al hijo
            fd_out_children[children_idx] = fd_out[PIPE_READ_END];    //leemos la info que nos devuelve el hijo
            }
    }

    // SE PUEDE HACER DE MANERA MAS EFICIENTE
    FILE * fd_in_opened[amount_of_children];
    FILE * fd_out_opened[amount_of_children];

    // Abrimos los archivos y modificamos el buffering
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        fd_in_opened[children_idx] = fdopen(fd_in_children[children_idx % amount_of_children], "w");
        fd_out_opened[children_idx] = fdopen(fd_out_children[children_idx % amount_of_children], "r");
        setvbuf(fd_in_opened[children_idx], NULL, _IONBF, 0);
        setvbuf(fd_out_opened[children_idx], NULL, _IONBF, 0);
    }
    
    // Distribuimos los archivos
    for (files_idx = 1, children_idx = 0; files_idx < argc; files_idx++, children_idx++) {
        fprintf(fd_in_opened[children_idx % amount_of_children], "%s\n", argv[files_idx]);
    }
    char buff[1024];

    printf("chau1\n");

    if (fscanf(fd_out_opened[0], "%s", buff) == EOF) {
        perror("Error reading from file descriptor");
    // Handle error appropriately
    }

    validate(waitpid(pid, &status, 0), WAITPID_ERROR_MSG);


    // Cerramos los archivos
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        fclose(fd_in_opened[children_idx]);
        fclose(fd_out_opened[children_idx]);
    }

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
