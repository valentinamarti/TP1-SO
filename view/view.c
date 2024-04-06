#include "view.h"

int main(int argc, char * argv[]) {
    size_t shm_lenght;
    int shm_fd;
    char shm_name[MAX_SIZE_BUFF];

    void * shm_memory_map = open_shm(PROT_READ, getpid(), &shm_lenght, &shm_fd, shm_name);

    char *token;

    while ((token = strtok((char *)shm_memory_map, "\n")) != NULL) {
        fprintf(stdout, "%s", token);
    }

}