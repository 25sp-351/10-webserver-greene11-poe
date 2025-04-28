#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "line.h"

#define INITIAL_BUFFER_SIZE 5

char* backslash_representation(char ch)
{
    switch (ch) {
    case '\0':
        return "\\0";
    case '\n':
        return "\\n";
    case '\r':
        return "\\r";
    case '\t':
        return "\\t";
    default:
        return "\\?";
    }
}
// return an allocated string; CRLF not present
char* read_http_line(int fd)
{
    // printf("Reading line from fd %d\n", fd);
    int alloced_size = INITIAL_BUFFER_SIZE;
    char* line = malloc(alloced_size);
    int len_read = 0;
    while (1) {
        char ch;
        if (len_read >= alloced_size) {
            alloced_size *= 2;
            line = realloc(line, alloced_size);
        }
        int number_bytes_read = read(fd, &ch, 1);
        if (number_bytes_read <= 0) {
            free(line);
            return NULL;
        }
        // ignore leading spaces
        if (len_read == 0 && ch == ' ') {
            continue;
        }
        // if (ch < 0x20) {
        //     printf("Read byte: '%s' (0x%02hhX)\n", backslash_representation(ch),
        //         ch);
        // } else {
        //     printf("Read byte: '%c' (0x%02hhX)\n", ch, ch);
        // }
        if (ch == '\n') {
            break;
        }
        line[len_read] = ch;
        len_read++;
        line[len_read] = '\0';
    }
    // remove trailing spaces and CR (\r)
    while (len_read > 0
           && (line[len_read - 1] == '\r' || line[len_read - 1] == ' ')) {
        line[len_read - 1] = '\0';
        len_read--;
    }
    line = realloc(line, len_read + 1);
    printf("Read line: '%s'\n", line);
    return line;
}
