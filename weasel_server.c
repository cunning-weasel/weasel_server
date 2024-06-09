#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <time.h>
#include <sys/socket.h>
#include <sys/mman.h>

#define PORT 8080
#define BUFFER_SIZE 60000
#define PATH_MAX 4096

// TO-DO: add logging. wouldn't need
// most printf and all perrors
// void log_message(const char *msg)
// {
//     time_t current_time;
//     struct tm *time_info;
//     time(&current_time);
//     time_info = localtime(&current_time);
//     printf("[%02d:%02d:%02d] %s\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec, msg);
// }

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
    size_t cap = 10000;

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

typedef struct Arena
{
    char *base; // void?
    char *used; // size_t?
    size_t size;
} Arena;

Arena *create_arena(size_t size)
{
    // -1 for no file mapping
    Arena *arena = (Arena *)mmap(NULL, sizeof(Arena) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (arena == MAP_FAILED)
    {
        perror("mmap call failure master weasel\n");
        exit(EXIT_FAILURE);
    }
    printf("mmap call success master weasel\n");

    arena->base = (char *)(arena + 1); // skip arena struct
    arena->used = arena->base;
    arena->size = size;

    return arena;
}

void *arena_allocate(Arena *arena, size_t size)
{
    // check space in arena
    if (arena->used + size > arena->base + arena->size)
    {
        perror("Not enough space in arena master weasel :/ )\n");
        return NULL;
    }
    // alloc more space from arena
    char *new_used = arena->used;
    arena->used += size;

    // return pointer to alloc'd block
    fprintf(stderr, "Allocated %zu bytes at %p\n", size, new_used);
    return new_used;
}

// void arena_reset(Arena *arena)
// {
//     arena->current = arena->base;
//     // clear the memory if needed (depending on a flag in Arena)?
//     // memset(arena->base, 0, arena->size);
// }

void arena_release(Arena *arena)
{
    if (munmap(arena, sizeof(Arena) + arena->size) == -1)
    {
        perror("munmap sys error master weasel\n");
        exit(EXIT_FAILURE);
    }
}

// TO-DO: dynamic buffer alloc?
// size_t get_request_size(int newsockfd)
// {
//     // get size of the incoming request
//     // ...
//     // ...
//     size_t buffer_size = get_request_size(newsockfd);
//     char *buffer = arena_allocate(arena, buffer_size);
// }

// TO-DO: roll own file handler
// need to look at sys calls
// typedef struct {
//     const char *extension;
//     const char *content_type;
// } ContentTypeMapping;

// // mappings
// ContentTypeMapping content_type_mappings[] = {
//     {".wasm", "application/wasm"},
//     {".js", "text/javascript"},
//     {".html", "text/html"},
//     // data file...
// };

// const char *get_content_type(const char *uri) {
//     // ...
// }

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
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // alloc for entire file
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
    }
    else
    {
        perror("fopen");
    }
}

// TO-DO: break up main into:
// setup_socket, parse_req, read_file, send_res
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

int main()
{
    size_t arena_size = 500 * 1024 * 1024; // 500MB
    Arena *arena = create_arena(arena_size);

    char *buffer = arena_allocate(arena, BUFFER_SIZE);
    if (!buffer)
    {
        perror("arena_allocate");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
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
        return 1;
    }
    printf("socket successfully bound to address\n");

    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
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
            close(newsockfd);
            continue;
        }

        sscanf(buffer, "%s %s %s", method, uri, version);

        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);

        read_file(arena, newsockfd, uri);
        close(newsockfd);
        // arena_reset(arena);
    }

    close(sockfd);
    arena_release(arena);
    return 0;
}
