#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 6000000
#define PATH_MAX 4096

size_t weasel_len(char *string)
{
    char *p = string;
    size_t len = 0;

    while (*p)
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
    size_t cap = 250000;

    if (start && str >= start && str <= end)
    {
        len = end - str;
        return len;
    }

    len = weasel_len(str);

    if (len > cap)
    {
        start = str;
        end = str + len;
    }

    return len;
}

void send_full_res(int newsockfd, char *content, char *content_type, size_t content_length)
{
    char header[1024];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n",
             content_type, content_length);

    int valwrite = write(newsockfd, header, custom_strlen_cacher(header));
    if (valwrite < 0)
    {
        perror("write");
        return;
    }

    valwrite = write(newsockfd, content, content_length);
    if (valwrite < 0)
    {
        perror("write");
        return;
    }
}

// TO-DO roll own filetype and handlers
void read_file(int newsockfd, char *uri)
{
    if (custom_strlen_cacher(uri) == 0 || (custom_strlen_cacher(uri) == 1 && uri[0] == '/'))
    {
        strcpy(uri, "/index.html");
    }

    if (uri[0] != '/')
    {
        char temp_uri[BUFFER_SIZE];
        snprintf(temp_uri, sizeof(temp_uri), "/%s", uri);
        strcpy(uri, temp_uri);
    }

    char filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "index%s", uri);

    FILE *fp = fopen(filepath, "rb");
    if (fp)
    {
        // determine the file size
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // alloc buffer for entire file
        // cast void pointer returned by malloc to char pointer
        // TO-DO roll own malloc/ arena
        char *buffer = (char *)malloc(file_size);
        if (!buffer)
        {
            perror("malloc");
            fclose(fp);
            return;
        }

        // read entire file into mem
        size_t bytes_read = fread(buffer, 1, file_size, fp);
        fclose(fp);

        if (bytes_read != file_size)
        {
            perror("fread");
            free(buffer);
            return;
        }

        char *content_type;
        if (strstr(uri, ".wasm"))
        {
            content_type = "application/wasm";
        }
        else if (strstr(uri, ".js"))
        {
            content_type = "text/javascript";
        }
        else
        {
            content_type = "text/html";
        }

        send_full_res(newsockfd, buffer, content_type, file_size);

        free(buffer);
    }
    else
    {
        perror("fopen");
    }
}

int main()
{
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (!buffer)
    {
        perror("malloc");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("webserver (socket)");
        free(buffer);
        return 1;
    }
    printf("socket created successfully\n");

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        free(buffer);
        return 1;
    }

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in client_addr;

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("webserver (bind)");
        free(buffer);
        return 1;
    }
    printf("socket successfully bound to address\n");

    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        free(buffer);
        return 1;
    }
    printf("server listening for connections on: http://localhost:8080 \n");

    while (1)
    {
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

        if (newsockfd < 0)
        {
            perror("webserver (accept) / set");
            continue;
        }
        printf("connection accepted and set\n");

        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(PORT);
        client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("read");
            close(newsockfd);
            continue;
        }

        char *method = malloc(BUFFER_SIZE);
        char *uri = malloc(BUFFER_SIZE);
        char *version = malloc(BUFFER_SIZE);

        if (!method || !uri || !version)
        {
            perror("malloc");
            free(method);
            free(uri);
            free(version);
            close(newsockfd);
            continue;
        }

        sscanf(buffer, "%s %s %s", method, uri, version);

        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method,
               version, uri);

        read_file(newsockfd, uri);

        free(method);
        free(uri);
        free(version);

        close(newsockfd);
    }

    free(buffer);
    close(sockfd);
    return 0;
}
