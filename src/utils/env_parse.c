#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *d_getenv(const char *file, const char *name) {
    FILE *fp;
    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Error while opening file");
        exit(EXIT_FAILURE);
    }

    // finds the length of the env value
    signed char c;
    int n = strnlen(name, 1000);
    // valid: if we're on a valid line
    // found: if we found the name
    // offset: how far the value is in the file
    // cur: how far we currently are in the file
    // count: index of name
    // env_len: length of the variable name
    int val = 1, found = 0, offset = 0, cur = 0, count = 0, env_len = 0;
    while ((c = fgetc(fp)) != EOF) {
        cur++;
        if (found)
            env_len++;
        if (c == '\n') {
            val = 1;
            if (found) // completely counted our value
                break;
            continue;
        }
        if (!found && val && c == '=') { // if we see a = and are valid, we found the right line
            found = 1;
            offset = cur;
        } else if (val) { // compares the line with the name value
            if (count < n && c == name[count])
                count++;
            else
                val = 0;
        }
    }
    if (env_len == 0)
        return NULL;

    // gets the env value
    char *res = (char *)malloc(env_len);
    fseek(fp, offset, SEEK_SET);
    for (int i = 0; i < env_len - 1; i++) {
        c = fgetc(fp);
        res[i] = c;
    }
    res[env_len - 1] = '\0';
    return res;
}
