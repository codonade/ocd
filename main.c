#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define strequals !strcmp
char *joins(char **words, size_t count) {
    size_t i, result_length = 0;
    char *p, *result;

    /* - compute the length of the resulting string. */
    for (i = 0; i < count; ++i) {
        result_length += strlen(words[i]);
        if (i + 1 < count) ++result_length;
    }

    /* - join all the words, separate them by a space. */
    result = malloc(result_length);
    p = result;
    for (i = 0; i < count; ++i) {
        size_t length = strlen(words[i]);
        memcpy(p, words[i], length);
        p += length;
        if (i + 1 < count) *p++ = ' ';
    }
    return result;
}

int main(int argc, char **argv) {
    char *command;
    int i, command_argi = 0;
    struct stat file_stat;
    time_t last_changed_time = 0;
    char buffer[128];

    /* - parse command line arguments. */
    for (i = 1; i < argc; ++i) {
        if (strequals(argv[i], "-do")) {
            command_argi = i + 1;
            break;
        }
    }
    if (!command_argi || command_argi < 2 || argc < 4) {
        fprintf(stderr, "USAGE: ocd path -do (command)\n");
        return (1);
    }
    command = joins(argv + command_argi, argc - command_argi);

    /* - watch for any changes in the contents of `path` */
    while (1) {
        if (!stat(argv[1], &file_stat)) {
            if (file_stat.st_mtime > last_changed_time) {
                FILE *pipe = popen(command, "r");
                system("clear");
                while (fgets(buffer, sizeof(buffer), pipe)) {
                    printf("%s", buffer);
                }
            }
            last_changed_time = file_stat.st_mtime;
        }
    }
    return (0);
}
