#include "sender.h"
#include <iostream>
#include <unistd.h>

Sender::Sender(sockaddr_in tAddr){
    tSocketAddr = tAddr;
    active = false;

    tSocketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (tSocketFd < 0){
        std::cerr << "Create sender socket failed" << "\n";
        exit(0);
    }
}

Sender::~Sender(){
    close(tSocketFd);
}

ssize_t Sender::send(const std::string & m){
    if (!active)
        return -1;
    ssize_t ret = sendto(tSocketFd, m.c_str(), MAX_LENGTH, 0, 
        reinterpret_cast<struct sockaddr*>(&tSocketAddr), sizeof(tSocketAddr));

    if (ret < 0)
        std::cerr << "Send failed" << "\n";
    return ret;
}

void Sender::start(){
    active = true;
}

void Sender::stop(){
    active = false;
}