#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
// made custom <string.h>
#include <sys/socket.h>
#include <unistd.h>
// bring in resp string/ index header
#include "index_html_string.h"

#define PORT 8080
#define BUFFER_SIZE 1024

// TO-DO
// add env vars and CD
// add proxy/ something to deal with ssl
// read html file instead of c string

// custom strlen - doesn't traverse
// string and trashe cache anymore
size_t weasel_len(char *string)
{
    char *p = string;
    size_t len = 0;

    while (*p != '\0')
    {
        len++;
        p++;
    }
    return len;
}

size_t custom_strlen_cacher(char *str)
{
    static char *start = NULL;
    static char *end = NULL;
    size_t len = 0;
    size_t cap = 5000; // 5kb

    // if we have a cached string and current pointer is within it
    if (start && str >= start && str <= end)
    {
        // calculate the new strlen
        len = end - str;
        // super-fast return!
        return len;
    }

    // count actual length
    len = weasel_len(str);

    // if it's a really long string, cache
    if (len > cap)
    {
        start = str;
        end = str + len;
    }
    // non-cached return
    return len;
}

int main()
{
    char buffer[BUFFER_SIZE];

    // man 2 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        // man perror
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");

    // man 2 socket
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    // man 2 bind
    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("webserver (bind)");
        return 1;
    }
    printf("socket successfully bound to address\n");

    // man 2 listen
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening for connections on: http://localhost:8080/ \n");

    while (1)
    {
        // man 2 accept
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0)
        {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        // get client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0)
        {
            perror("webserver (getsockname)");
            continue;
        }

        // man 2 socket
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
            continue;
        }

        // man 2 read
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        // TODO develop custom sscanf
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        // man 2 write
        int valwrite = write(newsockfd, resp, custom_strlen_cacher(resp));
        if (valwrite < 0)
        {
            perror("webserver (write)");
            continue;
        }
        // man 2 close
        close(newsockfd);
    }

    return 0;
}
