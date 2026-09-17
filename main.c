#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    struct stat file_stat;
    time_t last_changed_time = 0;

    char buffer[128];
    while (1) {
        if (!stat(argv[1], &file_stat)) {
            if (file_stat.st_mtime > last_changed_time) {
                FILE *pipe = popen(argv[2], "r");
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
