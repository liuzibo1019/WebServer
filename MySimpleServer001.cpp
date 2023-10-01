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
    // atoi convert string to integer
    int port = atoi(argv[2]);

    // 创建socket描述符, 参数依次为协议域, socket的类型SOCK_STREAM即TCP, 协议0表示使用默认协议
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 1);

    // 地址结构
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    // 设置地址族, AF_INET为IPV4
    address.sin_family = AF_INET;
    // 设置端口号, htons用于网络字节序与主机字节序之间转换的函数
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int ret = 0;
    // bind用于将socket返回的文件描述符与地址进行绑定
    ret = bind(listenfd, (struct sockaddr *)(&address), sizeof(address));
    // printf("bind: %d", ret);
    assert(ret != -1);

    // listen 对socket返回的文件描述符进行监听
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