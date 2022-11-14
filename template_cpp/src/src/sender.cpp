#include "sender.h"
#include <iostream>
#include <unistd.h>

Sender::Sender(std::vector<sockaddr_in> tAddrs){
    tSocketAddrs = tAddrs;
    active = false;
    size = static_cast<int>(tSocketAddrs.size());

    for (int i = 0; i < size; ++i){
        int tSocketFd = socket(AF_INET, SOCK_DGRAM, 0);

        if (tSocketFd < 0){
            std::cerr << "Create sender socket failed" << "\n";
            //exit(0);
        }

        tSocketFds.push_back(tSocketFd);
    }
}

Sender::~Sender(){
    for (int i = 0; i < size; ++i)
        close(tSocketFds[i]);
}

ssize_t Sender::send(const std::string & m, int id){
    if (!active)
        return -1;
    ssize_t ret = sendto(tSocketFds[id-1], m.c_str(), MAX_LENGTH, 0, 
        reinterpret_cast<struct sockaddr*>(&tSocketAddrs[id-1]), sizeof(tSocketAddrs[id-1]));

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