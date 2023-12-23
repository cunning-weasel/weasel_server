#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/mman.h>
#include <bits/mman-linux.h>

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

// arena
typedef struct Arena
{
    char *base;
    char *current;
    size_t size;
} Arena;

Arena *create_arena(size_t size)
{
    // pass -1 flag as we're not ding file mapping
    Arena *arena = (Arena *)mmap(NULL, sizeof(Arena) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (arena == MAP_FAILED)
    {
        perror("sys call mmap failure master weasel");
        exit(EXIT_FAILURE);
    }
    printf("syscall mmap success master weasel");

    arena->base = (char *)(arena + 1); // skip arena struct
    arena->current = arena->base;
    arena->size = size;

    return arena;
}

void *arena_allocate(Arena *arena, size_t size)
{
    // check space in arena
    if (arena->current + size > arena->base + arena->size)
    {
        perror("Not enough space in arena");
        return NULL;
    }
    // alloc more space from arena
    char *new_current = arena->current;
    arena->current += size;

    // return pointer to alloc'd block
    return new_current;
}

void arena_release(Arena *arena)
{
    if (munmap(arena, sizeof(Arena) + arena->size) == -1)
    {
        perror("munmap sys error master weasel");
        exit(EXIT_FAILURE);
    }
}

// TO-DO roll own file handler
void read_file(Arena *arena, int newsockfd, char *uri)
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
        // TO-DO roll own arena with mmap/ brk syscalls
        char *buffer = (char *)arena_allocate(arena, file_size);
        if (!buffer)
        {
            perror("arena_allocate");
            fclose(fp);
            return;
        }

        // read entire file into mem
        size_t bytes_read = fread(buffer, 1, file_size, fp);
        fclose(fp);

        if (bytes_read != file_size)
        {
            perror("fread");
            // free(buffer);
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

        // free(buffer);
        arena_release(arena);
    }
    else
    {
        perror("fopen");
    }
}

int main()
{
    // arena init
    size_t arena_size = 60 * 1024 * 1024; // 60MB
    Arena *arena = create_arena(arena_size);

    // char *buffer = (char *)malloc(BUFFER_SIZE);
    // if (!buffer)
    // {
    //     perror("malloc");
    //     return 1;
    // }
    char *buffer = arena_allocate(arena, BUFFER_SIZE);
    if (!buffer)
    {
        perror("ArenaAllocate");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("webserver (socket)");
        // free(buffer);
        return 1;
    }
    printf("socket created successfully\n");

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        // free(buffer);
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
        // free(buffer);
        return 1;
    }
    printf("socket successfully bound to address\n");

    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        // free(buffer);
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

        char *method = arena_allocate(arena, BUFFER_SIZE);
        char *uri = arena_allocate(arena, BUFFER_SIZE);
        char *version = arena_allocate(arena, BUFFER_SIZE);

        if (!method || !uri || !version)
        {
            perror("arena_allocate");
            // free(method);
            // free(uri);
            // free(version);
            close(newsockfd);
            continue;
        }

        sscanf(buffer, "%s %s %s", method, uri, version);

        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);

        read_file(arena, newsockfd, uri);

        // free(method);
        // free(uri);
        // free(version);

        close(newsockfd);
    }

    // free(buffer);
    arena_release(arena);

    close(sockfd);
    return 0;
}
