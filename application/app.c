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

    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {

        int fd_in[PIPE_FILEDESCRIPTORS];
        int fd_out[PIPE_FILEDESCRIPTORS];

        validate(pipe(fd_in), "ERROR: error when creating pipe");

        validate(pipe(fd_out), "ERROR: error when creating pipe");

        pid = fork();
        validate(pid, "ERROR: error when forking");
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


            // close(fd_out[PIPE_READ_END]);
            // close(STDOUT);
            // dup(fd_out[PIPE_WRITE_END]);
            // close(fd_out[PIPE_WRITE_END]);

            validate(execve("./childx", child_argv, child_env), "ERROR: error when executing child");
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
    FILE * fd_opened[amount_of_children];

    // Abrimos los archivos y modificamos el buffering
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        fd_opened[children_idx] = fdopen(fd_in_children[children_idx % amount_of_children], "w");
        setvbuf(fd_opened[children_idx], NULL, _IONBF, 0);
    }
    
    // Distribuimos los archivos
    for (files_idx = 1, children_idx = 0; files_idx < argc; files_idx++, children_idx++) {
        fprintf(fd_opened[children_idx % amount_of_children], "%s\n", argv[files_idx]);
    }

    // Cerramos los archivos
    for (children_idx = 0; children_idx < amount_of_children; children_idx++) {
        fclose(fd_opened[children_idx]);
    }

    for(children_idx = 0; children_idx < amount_of_children; children_idx++) {
        validate(waitpid(pid_children[children_idx], &status, 0), "ERROR: error when waiting for child process");
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

void validate(int code, char * message) {
    if (code == -1) {
        perror(message);
        exit(errno);
    }
}