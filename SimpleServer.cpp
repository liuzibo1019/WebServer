#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define MAX_EVENTS_NUMBER 5

int set_non_blocking(int fd)
{
    int old_state = fcntl(fd, F_GETFL);
    int new_state = old_state | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_state);

    return old_state;
}

void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

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

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int ret = 0;
    ret = bind(listenfd, (struct sockaddr *)(&address),
               sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENTS_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd);

    while (1)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENTS_NUMBER, -1);
        if (number < 0)
        {
            printf("epoll_wait failed\n");
            return -1;
        }

        for (int i = 0; i < number; ++i)
        {
            const auto &event = events[i];
            const auto eventfd = event.data.fd;

            if (eventfd == listenfd)
            {
                struct sockaddr_in client;
                socklen_t client_addrlength = sizeof(client);
                int sockfd = accept(listenfd, (struct sockaddr *)(&address),
                                    &client_addrlength);
                addfd(epollfd, sockfd);
            }
            else if (event.events & EPOLLIN)
            {
                char buf[1024] = {0};
                while (1)
                {
                    memset(buf, '\0', sizeof(buf));
                    int recv_size = recv(eventfd, buf, sizeof(buf), 0);
                    if (recv_size < 0)
                    {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        printf(" sockfd %d,recv msg failed\n", eventfd);
                        close(eventfd);
                        break;
                    }
                    else if (recv_size == 0)
                    {
                        close(eventfd);
                        break;
                    }
                    else
                    {
                        send(eventfd, buf, recv_size, 0);
                    }
                }
            }
        }
    }

    close(listenfd);

    return 0;
}
