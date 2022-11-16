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
        pending.push_back(msg);

        if(p != nullptr){
            std::string logMsg = 'b' + std::to_string(i);
            p->addLog(logMsg);
        }

        std::cout << pid << " fiforb broadcast " << msg << "\n";
        urb->broadcast(msg);
    }
}
