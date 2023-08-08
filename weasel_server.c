#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// bring in resp string/ index header
#include "index_html_string.h"

#define PORT 8080
#define BUFFER_SIZE 1024

// TODO develop custom strlen - currently traverses string and trashes cache
// size_t string_len(char *string)
// {
//     char *p = string;

//     while (*p != '\0')
//     {
//         p++;
//     }
//     return p - string; // num of chars advanced over
// }

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
        // int valwrite = write(newsockfd, resp, string_len(resp));
        int valwrite = write(newsockfd, resp, strlen(resp));
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
