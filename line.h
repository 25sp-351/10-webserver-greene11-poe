#ifndef LINE_H
#define LINE_H

// return an allocated string; CRLF not present
// returns NULL if EOF
// returns empty string if only a CRLF is present
// tolerates LF with CR (or without)
// strips leading and trailing spaces
char* read_http_line(int fd);

#endif
