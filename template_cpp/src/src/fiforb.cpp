#include "fiforb.h"
#include <iostream>
#include <string>

Fiforb::Fiforb(int _pid, std::vector<int> _targetPids, Urb* _urb, Process* _p){
    pid = _pid;
    targetPids = _targetPids;
    urb = _urb;
    p = _p;
    lsn = 0;
    numP = static_cast<int>(targetPids.size());
    for (int i = 0; i < numP; ++i)
        next.push_back(1);
    active = false;
}

Fiforb::~Fiforb(){}

void Fiforb::start(){
    active = true;
    if (urb != nullptr)
        urb->start();
}

void Fiforb::stop(){
    active = false;
    if (urb != nullptr)
        urb->stop();
}

void Fiforb::addMsg(unsigned long num){
    numM = static_cast<int>(num);
    
    for (int i = 1; i <= numM; ++i){
        lsn += 1;
        char msg[MAX_LENGTH] = {0};
        sprintf(msg, "%-d%c%03lu%-d", lsn, '-', static_cast<unsigned long>(pid), i);

        if(p != nullptr){
            std::string logMsg = 'b' + std::to_string(i);
            p->addLog(logMsg);
        }

        //std::cout << pid << " fiforb broadcast " << msg << "\n";
        urb->broadcast(msg);
    }
}

void Fiforb::broadcast(const std::string & msg){
    if (!active){
        std::cerr << "fiforb tried to broadcast when stop" << "\n";
        return;
    }
    lsn += 1;
    char head[MAX_LENGTH] = {0};
    sprintf(head, "%-d%c%03lu", lsn, '-', static_cast<unsigned long>(pid));
    if(p != nullptr){
        std::string logMsg = 'b' + msg;
        p->addLog(logMsg);
    }
    //std::cout << pid << " fiforb broadcast " << head+msg << "\n";
    urb->broadcast(head + msg);
}

void Fiforb::deliverLower(const std::string & msg){
    if (!pending.count(msg))
        pending.insert(msg);
    size_t pos = msg.find('-');
    int s = stoi(msg.substr(pos + 1, 3));

    //std::cout << "fifob delivered lower " << msg <<"\n";
    std::set<std::string> rem;
    for (auto& hm: pending){
        size_t pos2 = hm.find('-');
        if (pos >= hm.size()){
            std::cerr << "wtf with the fifo msg " << hm << "\n";
        }
        int sn2 = stoi(hm.substr(0, pos2));
        int s2 = stoi(hm.substr(pos2 + 1, 3));
        if (s2 == s and sn2 == next[s-1]) {
            next[s-1] += 1;
            rem.insert(hm);
            //std::cout << pid << " fifob deliver " << hm << "\n";
            if (p != nullptr){
                std::string logMsg = 'd' + hm.substr(pos2 + 1, hm.size());
                p->addLog(logMsg);
            }
        }
    }
    for (auto& r : rem){
        pending.erase(r);
    }
}