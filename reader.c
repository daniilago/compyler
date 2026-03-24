#include "reader.h"
#include <unistd.h>  
#include <fcntl.h>    
#include <stdlib.h>   

char *read_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    if (!buf) { close(fd); return NULL; }

    read(fd, buf, size);
    buf[size] = '\0';

    close(fd);
    return buf;
}

void free_file(char *buf) {
    free(buf);
}