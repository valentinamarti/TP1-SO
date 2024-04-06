#include "utils.h"

void validate(int code, char * message) {
    if (code == -1) {
        perror(message);
        exit(errno);
    }
}