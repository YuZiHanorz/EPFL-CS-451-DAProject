#include "perfectLink.h"

#include <iostream>

PerfectLink::PerfectLink(int _pid, int _targetPid, Receiver* _rec, Sender* _sen, Process* _p){
    pid = _pid;
    targetPid = _targetPid;
    rec = _rec;
    sen = _sen;
    p = _p;
    active = false;

    sendThread = std::thread(&PerfectLink::sendT, this);
    recThread = std::thread(&PerfectLink::recT, this);
    ackThread = std::thread(&PerfectLink::ackT, this);
}

PerfectLink::~PerfectLink(){}

void PerfectLink::start(){
    active = true;
    if (sen != nullptr)
        sen->start();
}

void PerfectLink::stop(){
    active = false;
    if (sen != nullptr)
        sen->stop();
}

// incured by main
void PerfectLink::addMsg(const std::string msg){
    pendingMsgs.push_back(msg);
}

void PerfectLink::addMsgs(const std::list<std::string> msgs){
    for (auto& msg: msgs)
        pendingMsgs.push_back(msg);
    
}

void PerfectLink::sendT(){
    while (true){
        if (!active)
            continue;
        
        if (sen != nullptr){
            std::lock_guard<std::mutex> lock(msgMutex);
            
            if (!pendingMsgs.empty()){
                for (auto& m: pendingMsgs){
                    if(p != nullptr){
                        std::lock_guard<std::mutex> lock(logMutex);
                        std::string logMsg = 'b' + m.substr(1,m.size());
                        p->addLog(logMsg);
                    }
                    std::cout << pid << " send msg " << m << " to " << targetPid << "\n";
                    sen->send(m);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void PerfectLink::recT(){
    while (true){
        if (!active)
            continue;
        
        char buffer[MAX_LENGTH];
        if (rec->receive(buffer) >= 0){  //what about =0?
            std::string res{buffer};
            std::string ack = res.substr(0, 1);
            int msgSpid = stoi(res.substr(1, 3));
            std::string payload = res.substr(4, res.size());
            std::string deliverMsg = res.substr(1, res.size());

            if(targetPid == msgSpid &&  ack == "0"){ //receive a msg from the other end of the link
                if (p != nullptr){
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::string logMsg = 'd' + deliverMsg;
                    p->addLog(logMsg);
                }
                std::cout << pid << " receive " << deliverMsg << " from " << targetPid << "\n";
                res[0] = '1';
                std::lock_guard<std::mutex> lock(ackMutex);
                pendingAcks.push_back(res);
            }
            else if (pid == msgSpid && ack == "1"){ //receive an ack from the other end
                std::cout << pid << " receive ack " << deliverMsg << " from " << targetPid << "\n";
                res[0] = '0';
                std::lock_guard<std::mutex> lock(msgMutex);
                pendingMsgs.remove(res);
            }
            else 
                std::cerr << "receive unexpected message "<< res << " with " << pid << targetPid << msgSpid << ack <<"\n"; 
            
        }
    }
}

void PerfectLink::ackT(){
    while (true){
        if (!active)
            continue;
        
        if (sen != nullptr){
            std::lock_guard<std::mutex> lock(ackMutex);
            pendingAcks.unique();
            if (!pendingAcks.empty()){
                for (auto& a: pendingAcks){
                    std::cout << pid << " send ack " << a << " to " << targetPid << "\n";
                    sen->send(a);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                pendingAcks.clear();
            }
        }
    }
}




