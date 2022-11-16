#include "receiver.h"
#include <iostream>
#include <unistd.h>
#include <string.h>

Receiver::Receiver(in_addr_t ip, unsigned short port){
    active = false;

    // socket init
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0){
        std::cerr << "Create receiver socket failed" << "\n";
        exit(0);
    }

    socketAddr.sin_addr.s_addr = ip;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = port;

    socklen_t socketAddrLen = sizeof(socketAddr);
    memset(socketAddr.sin_zero, '\0', sizeof(socketAddr.sin_zero));
    if (bind(socketFd, reinterpret_cast<struct sockaddr* const>(&socketAddr), socketAddrLen) < 0) {
        std::cerr << "Bind socket failed" << "\n";
        exit(0);
    }
}

Receiver::~Receiver(){
    close(socketFd);
}

ssize_t Receiver::receive(char* buffer){
    if (!active)
        return -1;
    struct sockaddr_in sAddr;
    socklen_t sAddrLen;
    char tmpBuffer[MAX_LENGTH];

    ssize_t ret = recvfrom(socketFd, tmpBuffer, sizeof(tmpBuffer), 0, 
        reinterpret_cast<struct sockaddr*>(&sAddr), &sAddrLen);

    if (ret < 0)
        std::cerr << "Receive failed " << ret << "\n";
    else
        strncpy(buffer, tmpBuffer, sizeof(tmpBuffer));
    
    return ret;
}

void Receiver::start(){
    active = true;
}

void Receiver::stop(){
    active = false;
}
