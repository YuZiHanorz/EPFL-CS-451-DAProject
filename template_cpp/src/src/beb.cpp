#include "beb.h"
#include "latagr.h"

#include <iostream>

Beb::Beb(int _pid, std::vector<int> _targetPids, PerfectLink * _pl, Process* _p){
    pid = _pid;
    targetPids = _targetPids;
    pl = _pl;
    p = _p;
    active = false;
}

Beb::~Beb(){}

void Beb::setUpper(LatticeAgr* _lag){
    lag = _lag;
}

void Beb::start(){
    active = true;
    if (pl != nullptr)
        pl->start();
}

void Beb::stop(){
    active = false;
    if (pl != nullptr)
        pl->stop();
}

void Beb::addMsg(unsigned long num){
    int nump = static_cast<int>(targetPids.size());
    bool flag = true;
    for (int i = 0; i < nump; ++i){
        int tPid = targetPids[i];
        if (tPid == pid)
            continue;
        for (unsigned int j = 1; j <= num; ++j){
            char msg[MAX_LENGTH] = {0};
            int ack = 0;
            sprintf(msg, "%-1d%03lu%03lu%-d", ack, static_cast<unsigned long>(pid), static_cast<unsigned long>(tPid), j);
            pl->addMsg(msg);
            if(p != nullptr && flag == true){
                std::string res(msg);
                std::string logMsg = 'b' + res.substr(1, res.size());
                p->addLog(logMsg);
            }
            std::cout << pid << "beb add msg " << msg << "\n";
        }
        flag = false;
    }
}

void Beb::broadcast(const std::string& msg){
    if (!active){
        std::cerr << "beb tried to broadcast when stop" << "\n";
        return;
    }
    int nump = static_cast<int>(targetPids.size());
    for (int i = 0; i < nump; ++i){
        int tPid = targetPids[i];
        if (tPid == pid)
            continue;
        char hm[MAX_LENGTH] = {0};
        int ack = 0;
        sprintf(hm, "%-1d%03lu%03lu", ack, static_cast<unsigned long>(pid), static_cast<unsigned long>(tPid));
        std::string hmp(hm);
        pl->addMsg(hmp + msg);
    }
}

void Beb::deliver(const std::string & msg){
    lag->deliverLower(msg);
}