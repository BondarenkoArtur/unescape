#include <stdio.h>

#define BUFFER_SIZE 1024

const char *unescape(const char *, char *, size_t);

int main() {
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        printf("%s", unescape(buffer, buffer, sizeof(buffer)));
    }
}