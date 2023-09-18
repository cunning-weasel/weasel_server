#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <signal.h>
// resp string/ index header
#include "index_html_string.h"

#define PORT 8080
#define BUFFER_SIZE 1024

/*
TO-DO
remove nginx
read html file instead of c string
blog articles end-point
back-end pagination
CD
*/

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context master weasel");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

// std strlen trashes cache
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

// cache a single item
size_t custom_strlen_cacher(char *str)
{
    static char *start = NULL;
    static char *end = NULL;
    size_t len = 0;
    size_t cap = 5000; // 5kb

    // str is cached and current pointer is within it
    if (start && str >= start && str <= end)
    {
        // calc new strlen
        len = end - str;
        return len;
    }

    // calc actual length
    len = weasel_len(str);

    // do cache
    if (len > cap)
    {
        start = str;
        end = str + len;
    }
    // un-cached return
    return len;
}

int main()
{
    char buffer[BUFFER_SIZE];

    SSL_CTX *ctx;
    SSL *ssl;
    SSL_library_init();

    ctx = create_context();
    configure_context(ctx);
    // ignore broken pipe signals
    signal(SIGPIPE, SIG_IGN);

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
    printf("server listening for connections on: https://localhost:8080 \n");

    while (1)
    {
        ssl = SSL_new(ctx);
        // BIO pair needed for reading and writing
        BIO *ssl_bio_read = BIO_new(BIO_s_mem());
        BIO *ssl_bio_write = BIO_new(BIO_s_mem());

        SSL_set_bio(ssl, ssl_bio_read, ssl_bio_write);

        if (SSL_accept(ssl) <= 0)
        {
            ERR_print_errors_fp(stderr);
        }

        // man 2 accept
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        SSL_set_fd(ssl, newsockfd);

        if (newsockfd < 0)
        {
            perror("webserver (accept) / ssl (set)");
            continue;
        }
        printf("connection accepted and ssl set\n");

        // get client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0)
        {
            perror("webserver (getsockname)");
            continue;
        }

        // man 2 socket
        // int valread = read(newsockfd, buffer, BUFFER_SIZE);
        int valread = SSL_read(ssl, buffer, BUFFER_SIZE);

        /*
         compare paths here to serve blog, 404 etc
        */

        if (valread < 0)
        {
            perror("ssl (read)");
            continue;
        }

        // man 2 read
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];

        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        // man 2 write
        // int valwrite = write(newsockfd, resp, custom_str_len(resp));
        int valwrite = SSL_write(ssl, resp, custom_strlen_cacher(resp));
        if (valwrite < 0)
        {
            perror("sll (write)");
            continue;
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        // man 2 close
        close(newsockfd);
    }
    SSL_CTX_free(ctx);
    return 0;
}

// docs:
// https://wiki.openssl.org/index.php/Simple_TLS_Server
// https://github.com/openssl/openssl/blob/7a5f58b2cf0d7b2fa0451603a88c3976c657dae9/demos/sslecho/main.c#L295
