#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef int bool;
#define TRUE 1
#define FALSE 0

#define strequ !strcmp
char *join_by_space(char **words, size_t count) {
    size_t i, result_length = 0;
    char *p, *result;

    /* - compute the length of the resulting string. */
    for (i = 0; i < count; ++i) {
        result_length += strlen(words[i]);
        if (i + 1 < count) ++result_length;
    }

    /* - join all the words, separate them by a space. */
    result = malloc(result_length + 1);
    p = result;
    for (i = 0; i < count; ++i) {
        size_t length = strlen(words[i]);
        memcpy(p, words[i], length);
        p += length;
        if (i + 1 < count) *p++ = ' ';
    }
    *p = '\0';
    return result;
}

#define INCORRECT_USAGE 1
#define FILE_DOES_NOT_EXIST 2
int failure(int error, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
    return error;
}

#define UNKNOWN_ERROR_OCCURRED -1
int unknown_failure() {
    return failure(
        UNKNOWN_ERROR_OCCURRED,
        "An unknown error (%d) occurred!",
        errno);
}

int main(int argc, char **args) {
    int i, files_count, command_argi = 0, error;
    char *command;
    struct stat *stats;
    bool is_first_run = TRUE;
    char buffer[128];

    /* - parse command line arguments. */
    for (i = 1; i < argc; ++i) {
        if (strequ(args[i], "--command") || strequ(args[i], "-C")) {
            command_argi = i + 1;
            break;
        }
    }
    if (!command_argi || command_argi < 3 || argc < 4)
        return failure(INCORRECT_USAGE, "USAGE: ocd [paths] --command (command)");
    files_count = command_argi - 2;
    command = join_by_space(args + command_argi, argc - command_argi);

    /* - ensure all passed files exist. */
    stats = malloc(sizeof(*stats) * files_count);
    for (i = 0; i < files_count; ++i) {
        char *file = args[i + 1];
        error = stat(file, &stats[i]);
        if (error) {
            if (errno == ENOENT)
                return failure(FILE_DOES_NOT_EXIST, "File %s doesn't exit!", file);
            else return unknown_failure();
        }
    }

    /* - watch all files for any kind of change. */
    while (1) {
        for (i = 0; i < files_count; ++i) {
            char *file = args[i + 1];
            struct stat file_stat;
            error = stat(file, &file_stat);
            if (error && errno != ENOENT) return unknown_failure();
            if (stats[i].st_mtime < file_stat.st_mtime || is_first_run) {
                FILE *pipe = popen(command, "r");
                system("clear");
                printf("%s\n\n", command);
                is_first_run = FALSE;
                while (fgets(buffer, sizeof(buffer), pipe)) {
                    printf("%s", buffer);
                }
            }
            stats[i] = file_stat;
        }
    }

    return 0;
}
