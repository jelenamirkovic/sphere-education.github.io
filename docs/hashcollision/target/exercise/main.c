#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include "hash.h"

int main(int argc, char *argv[])
{
    char *key;
    unsigned int key_len;
    unsigned int r;
    int contains_key;
    if (argc < 2) {
        fprintf(stderr, "usage: %s key_len\n", argv[0]);
        return EXIT_FAILURE;
    }
    key_len = (unsigned int)atoi(argv[1]);
    hset_init();
    key = malloc(sizeof (char) * key_len);
    while (1) {
        r = read(STDIN_FILENO, key, key_len);
        if (r == -1) {
            fprintf(stderr, "read error: %s\n", strerror(errno));
        }
        if (r < key_len) {
            printf("Received too short key of len %d (not %d), exiting\n", r, key_len);
            break;
        }
        if (key[r - 1] != '\n') {
            printf("Expected newline character, exiting\n");
            break;
        }
        key[r - 1] = '\0';
        hset_add(key);
        contains_key = hset_contains(key);
        printf("hset contains: %s: %d\n", key, contains_key);
        
    }
    hset_dump();
    hset_free();
    free(key);
    return EXIT_SUCCESS;
}
