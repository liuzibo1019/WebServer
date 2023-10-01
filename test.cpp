#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

int main(int argc, char* argv[]){
    int port = 1;
    std::cout << port << std::endl;
    std::cout << htons(port) << std::endl;
    // assert(a == 2);
    return 0;
}