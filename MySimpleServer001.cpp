#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("Usage: %s ip_address portname\n", argv[0]);
        return 0;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 1);
    // printf("listenfd: %d", listenfd);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int ret = 0;
    ret = bind(listenfd, (struct sockaddr *)(&address), sizeof(address));
    // printf("bind: %d", ret);
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int sockfd = accept(listenfd, (struct sockaddr *)(&client), &client_addrlength);
    char buff[1024] = {0};
    char re[1024] = "hello socket, here is your message:\n";
    // printf("connecting to %s, port %d\n", inet_ntop(AF_INET, &address.sin_addr, buff, sizeof(buff)), ntohs(address.sin_port));
    // printf("connecting from %s, port %d\n", inet_ntop(AF_INET, &client.sin_addr, buff, sizeof(buff)), ntohs(client.sin_port));
    int recv_size = 0;
    recv_size = recv(sockfd, buff, sizeof(buff), 0);

    int send_size = 0;
    send(sockfd, re, sizeof(re), 0);
    send_size = send(sockfd, buff, recv_size, 0);
    close(sockfd);
    close(listenfd);
    return 0;
}