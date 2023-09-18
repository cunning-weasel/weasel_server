#include "index_html_string_header.h"

char resp_header
    [] = "HTTP/1.1 200 OK\r\n"
         "Server: webserver-c\r\n"
         "Content-Type: text/html\r\n"
         "Content-Length: %lu\r\n" // len of content
         "\r\n";