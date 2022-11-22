#include "perfectLink.h"
#include "beb.h"

#include <iostream>

PerfectLink::PerfectLink(int _pid, std::vector<int> _targetPids, Receiver* _rec, Sender* _sen){
    pid = _pid;
    targetPids = _targetPids;
    rec = _rec;
    sen = _sen;
    
    active = false;

    sendThread = std::thread(&PerfectLink::sendT, this);
    recThread = std::thread(&PerfectLink::recT, this);
}

PerfectLink::~PerfectLink(){}

void PerfectLink::setUpper(Beb* _beb){
    beb = _beb;
}

void PerfectLink::start(){
    active = true;
    if (rec != nullptr)
        rec->start();
    if (sen != nullptr)
        sen->start();
}

void PerfectLink::stop(){
    active = false;
    if (rec != nullptr)
        rec->stop();
    if (sen != nullptr)
        sen->stop();
}

// incured by main
void PerfectLink::addMsg(const std::string& msg){
    pendingMsgs.push_back(msg);
}

void PerfectLink::addMsgs(const std::list<std::string>& msgs){
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
                    std::string ack = m.substr(0, 1);
                    int msgSpid = stoi(m.substr(1, 3));
                    int msgTpid = stoi(m.substr(4, 3));
                    std::string deliverMsg = m.substr(1, m.size());
                    if (ack == "1" || msgSpid != pid || msgTpid == pid){
                        std::cerr << "wrong msg send: " << m;
                        exit(0);
                    }
                    //std::cout << pid << " send msg " << m << "\n";
                    sen->send(m, msgTpid);
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

        if (sen != nullptr){
            std::lock_guard<std::mutex> lock(ackMutex);

            pendingAcks.unique();
            if (!pendingAcks.empty()){
                for (auto& a: pendingAcks){
                    std::string ack = a.substr(0, 1);
                    int msgSpid = stoi(a.substr(1, 3));
                    int msgTpid = stoi(a.substr(4, 3));
                    std::string deliverMsg = a.substr(1, a.size());
                    if (ack == "0" || msgTpid != pid || msgSpid == pid){
                        std::cerr << "wrong ack send: " << a;
                        exit(0);
                    }
                    //std::cout << pid << " send ack " << a << "\n";
                    sen->send(a, msgSpid);
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                pendingAcks.clear();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
            int msgTpid = stoi(res.substr(4, 3));
            //std::string payload = res.substr(7, res.size());
            std::string deliverMsg = res.substr(1, res.size());

            if(pid == msgTpid &&  ack == "0"){ //receive a msg from the other end of the link
                if (beb != nullptr){
                    //std::lock_guard<std::mutex> lock(logMutex);
                    //std::string logMsg = 'd' + deliverMsg;
                    beb->deliver(deliverMsg);
                }
                //std::cout << pid << " receive " << res  << "\n";
                res[0] = '1';
                std::lock_guard<std::mutex> lock(ackMutex);
                pendingAcks.push_back(res);
            }
            else if (pid == msgSpid && ack == "1"){ //receive an ack from the other end
                //std::cout << pid << " receive ack " << res  << "\n";
                res[0] = '0';
                std::lock_guard<std::mutex> lock(msgMutex);
                pendingMsgs.remove(res);
            }
            else 
                std::cerr << "receive unexpected message "<< res << "\n"; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
