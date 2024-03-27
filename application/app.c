#include "app.h"

int main(int argc, char * argv[]) {
    
    pid_t pid;
    
    int status;

    int file_index = 0; 

    unsigned int amount_of_children = get_children_amount(argc-1);

    pid_t pid_children[amount_of_children];

    int fd_in[PIPE_FILEDESCRIPTORS];
    int fd_out[PIPE_FILEDESCRIPTORS];

    if (pipe(fd_in) == -1) {
        perror("ERROR: error when creating pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd_out) == -1) {
        perror("ERROR: error when creating pipe");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < amount_of_children; i++) {
        pid = fork();
        if(pid < 0){
            perror("ERROR: error when forking");
            exit(EXIT_FAILURE);
        } else {
            if(pid == 0){
                char * child_argv[] = {"child", NULL};
                char * child_env[] = {NULL};

                close(fd_in[PIPE_WRITE_END]);
                close(fd_out[PIPE_READ_END]);

                close(STDIN);
                dup(fd_in[PIPE_READ_END]);
                close(fd_in[PIPE_READ_END]);

                close(STDOUT);
                dup(fd_out[PIPE_WRITE_END]);
                close(fd_out[PIPE_WRITE_END]);
            
                if(execve("child", child_argv, child_env) == -1){
                    perror("ERROR: error when executing child");
                    exit(EXIT_FAILURE);
                }
            }
            close(fd_in[PIPE_READ_END]);
            close(fd_out[PIPE_WRITE_END]);

            close(STDOUT);
            dup(fd_in[PIPE_READ_END]);
            close(fd_in[PIPE_READ_END]);

            close(STDIN);
            dup(fd_out[PIPE_WRITE_END]);
            close(fd_out[PIPE_WRITE_END]);
        }
    }

    // Aca deberiamos hacer un ciclo que en base a las señales de los hijos le vaya mandando archivos de a poco.
    // Suponemos que con un select tenemos que ver a cual de los hijos ir mandando archivos
    // Para mandarle archivos a cada hijo deberiamos escribir en cada pipe en singular ? porque si mapeamos todos los pipes con la salida estandar, se nos va a "mezclar" toda la informacion

     for(int i = 0; i < amount_of_children; i++){
        if(waitpid(pid_children[i], &status, 0) == -1){    //WUNTRACED:if any of the child processes have stopped waitpid() will return immediately
            perror("ERROR: error when waiting for child process");
            exit(EXIT_FAILURE);
        }else{
            printf("The child %d has died\n", pid_children[i]); //Despues borrar
        }
    }

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