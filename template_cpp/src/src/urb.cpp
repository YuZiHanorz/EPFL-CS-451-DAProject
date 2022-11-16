#include "urb.h"
#include <iostream>
#include <string>

Urb::Urb(int _pid, std::vector<int> _targetPids, Beb* _beb, Process* _p){
    pid = _pid;
    targetPids = _targetPids;
    beb = _beb;
    p = _p;
    active = false;

    deliverThread = std::thread(&Urb::deliverT, this);
}

Urb::~Urb(){}

void Urb::start(){
    active = true;
    if (beb != nullptr)
        beb->start();
}

void Urb::stop(){
    active = false;
    if (beb != nullptr)
        beb->stop();
}

void Urb::addMsg(unsigned long num){
    numM = static_cast<int>(num);
    int numP = static_cast<int>(targetPids.size());
    for (int i = 1; i <= numM; ++i){
        char msg[MAX_LENGTH] = {0};
        sprintf(msg, "%03lu%-d", static_cast<unsigned long>(pid), i);
        pending.insert(msg);

        if(p != nullptr){
            std::string logMsg = 'b' + std::to_string(i);
            p->addLog(logMsg);
        }

        std::cout << pid << " urb broadcast " << msg << "\n";
        beb->broadcast(msg);
    }
}

void Urb::broadcast(const std::string & msg){
    
}


void Urb::deliverLower(const std::string & msg){
    std::cout << pid << " urb defliver from beb " << msg << "\n";
    int msgSpid = stoi(msg.substr(0, 3));
    int msgTpid = stoi(msg.substr(3, 3)); // = pid
    std::string sm = msg.substr(6, msg.size());
    //int s = stoi(msg.substr(6, 3));
    //int m = stoi(msg.substr(9, msg.size()));
    
    if (1){
        std::lock_guard<std::mutex> lock(ackMutex);
        std::string p = std::to_string(msgSpid);
        auto iter = ack.find(sm);
        if (iter != ack.end()){
            if (!iter->second.count(p))
                iter->second.insert(p);
        }
        else {
            std::set<std::string> st{p};
            ack[sm] = st;
        }
        std::cout << pid << " ack add " << sm  << " " << p << "\n";
    }
    //char sm[MAX_LENGTH] = {0};
    //sprintf(sm, "%03lu%-d", static_cast<unsigned long>(s), m);

    if (1){
        std::lock_guard<std::mutex> lock(penMutex);
        if (!pending.count(sm)){
            pending.insert(sm);
            std::cout << pid << " urb broadcast when deliver from beb " << sm << "\n";
            beb->broadcast(sm);
        }
    }
}

bool Urb::canDeliver(const std::string & sm){
    int N = static_cast<int>(targetPids.size());
    std::lock_guard<std::mutex> lock(ackMutex);
    return static_cast<int>(ack[sm].size()) > N / 2;
}

void Urb::deliverT(){
    while (true){
        if (!active)
            continue;

        std::set<std::string> pendings;
        if (1){
            std::lock_guard<std::mutex> lock(penMutex);
            pendings = pending;
        }
        for (auto& sm: pendings){
            //int s = stoi(sm.substr(0, 3));
            //int m = stoi(sm.substr(3, sm.size()));
            //int idx = (s-1) * numM + m - 1;
            if (!delivered.count(sm) && canDeliver(sm)){
                delivered.insert(sm);
                std::cout << pid << " urb deliver " <<sm << "\n";
                if (p != nullptr){
                    std::string logMsg = 'd' + sm;
                    p->addLog(logMsg);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}