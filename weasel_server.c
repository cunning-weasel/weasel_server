#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>weasel server says hi</html>\r\n";

    // man 7 tcp
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        // man perror
        perror("webserver (socket) oopsie");
        return 1;
    }
    printf("weasel socket created!!\n");

    // address to bind socket to
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    // man 7 ip
    host_addr.sin_family = AF_INET;
    // man 3 hton
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // man 2 bind
    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("webserver bind");
        return 1;
    }
    printf("socket successfully bound to address\n");

    // man 2 listen
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening on http://localhost:8080/!\n");

    // man 2 accept
    for (;;)
    {
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0)
        {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        // man 2 read
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
            continue;
        }

        // man 2 write
        // TODO write custom strlen
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

// docs:
// man 2 read
// man 7 ip
// man tcp 7
// man socket 2

// compile and link exec: gcc -o output_weasel_server weasel_server.c
// run compiled file: ./output_weasel_server
