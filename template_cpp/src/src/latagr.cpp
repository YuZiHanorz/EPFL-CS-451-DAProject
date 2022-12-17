#include "latagr.h"
#include <iostream>
#include <sstream>
#include <string>

LatticeAgr::LatticeAgr(int _pid, std::vector<int> _targetPids, Beb* _beb, Process* _p){
    pid = _pid;
    targetPids = _targetPids;
    beb = _beb;
    p = _p;
    active = false;
}

LatticeAgr::~LatticeAgr(){}

void LatticeAgr::start(){
    active = true;
    if (beb != nullptr)
        beb->start();
}

void LatticeAgr::stop(){
    active = false;
    if (beb != nullptr)
        beb->stop();
}

void LatticeAgr::propose(const std::vector<std::set<std::string>> & prop){
    if (!active){
        std::cerr << "lattice agreement tried to broadcast when stop" << "\n";
        return;
    }
    for (int i = 0; i < static_cast<int>(prop.size()); ++i){
        finished.push_back(false);
        std::set<std::string> st1, st2;
        ack.push_back(st1);
        nack.push_back(st2);
        active_prop_num.push_back(1);
        prop_val.push_back(prop[i]);
        accepted_val.push_back(prop[i]);
        std::string msg = "p" + std::to_string(i) + "-" + std::to_string(active_prop_num[i]) + "[";
        for (auto& pv : prop[i]){
            msg += pv + ",";
        }
        //std::cout << pid << " proposed " << msg << "\n";
        beb->broadcast(msg);
    }
}

void LatticeAgr::deliverLower(const std::string & msg){
    if (!active){
        std::cerr << "lattice agreement tried to broadcast when stop" << "\n";
        return;
    }
    //std::cout << "lattice deliver lower " << msg << "\n";
    int N = static_cast<int>(targetPids.size());
    int msgSpid = stoi(msg.substr(0, 3));
    int msgTpid = stoi(msg.substr(3, 3)); // = pid
    std::string tp = msg.substr(6, 1);

    if (tp == "p"){
        size_t pos = msg.find('-');
        int idx = stoi(msg.substr(7, pos-7));
        size_t pos2 = msg.find('[');
        int p_num = stoi(msg.substr(pos+1, pos2-pos-1));
        std::string pp = msg.substr(pos2+1, msg.size());
        std::stringstream ss(pp);
        std::string item;
        std::set<std::string> tmp;
        //std::cout << pid << "receive prop from " << msgSpid << " " << msg << "\n";
        while (std::getline(ss, item, ',')) {
            tmp.insert(item);
            //std::cout << "tmp propose " << item << "\n"; 
        }
        bool isack = true;
        for (auto& e: accepted_val[idx]){
            if (!tmp.count(e)){
                isack = false;
                tmp.insert(e);
            }
        }
        accepted_val[idx] = tmp;
        std::string mm;
        if (isack){
            mm = "a" + std::to_string(idx) + "-" + std::to_string(p_num) + "&" + std::to_string(msgSpid);
        }
        else{
            mm = "n" + std::to_string(idx) + "-" + std::to_string(p_num) + "&" + std::to_string(msgSpid) + "[";
            for (auto& pv : accepted_val[idx]){
                mm += pv + ",";
            }
        }
        //std::cout << pid << " send ack/nack " << mm << "\n";
        beb->broadcast(mm);
    }

    else if (tp == "a"){
        size_t pos = msg.find('-');
        int idx = stoi(msg.substr(7, pos-7));
        size_t pos2 = msg.find('&');
        int p_num = stoi(msg.substr(pos+1, pos2-pos-1));
        int pj = stoi(msg.substr(pos2+1, msg.size()));
        //std::cout << pj << " " << pid << " " << p_num << " " << active_prop_num[idx] << " " << ack[idx].size() << " " << nack[idx].size()<< "\n";
        if (pj == pid && p_num == active_prop_num[idx]){
            std::string ackpp = std::to_string(msgSpid);
            if (!ack[idx].count(ackpp))
                ack[idx].insert(ackpp);
            if (!finished[idx] && static_cast<int>(nack[idx].size()) > 0 && static_cast<int>(nack[idx].size()) + static_cast<int>(ack[idx].size()) >= N / 2){
                active_prop_num[idx] += 1;
                ack[idx].clear();
                nack[idx].clear();
                std::string nbm = "p" + std::to_string(idx) + "-" + std::to_string(active_prop_num[idx]) + "[";
                for (auto& pv : prop_val[idx]){
                    nbm += pv + ",";
                }
                accepted_val[idx] = prop_val[idx];
                //std::cout << pid << " nack trigger proposed " << nbm << "\n";
                beb->broadcast(nbm);
            }
            else if (!finished[idx] && static_cast<int>(ack[idx].size()) >= N / 2){
                finished[idx] = true;
                if (p != nullptr){
                    std::string logMsg = std::to_string(idx) + "d";
                    for (auto& pv : prop_val[idx])
                        logMsg += pv + ",";
                    //std::cout << pid << "decide " << idx << " " << logMsg << "\n";
                    p->addLog(logMsg);
                }
            }
        }
    }

    else if (tp == "n"){
        size_t pos = msg.find('-');
        int idx = stoi(msg.substr(7, pos-7));
        size_t pos2 = msg.find('&');
        int p_num = stoi(msg.substr(pos+1, pos2-pos-1));
        size_t pos3 = msg.find('[');
        int pj = stoi(msg.substr(pos2+1, pos3-pos2-1));
        //std::cout << pj << " " << pid << " " << p_num << " " << active_prop_num[idx] << " " << ack[idx].size() << " " << nack[idx].size()<< "\n";
        if (pj == pid && p_num == active_prop_num[idx]){
            std::string pp = msg.substr(pos3+1, msg.size());
            std::stringstream ss(pp);
            std::string item;
            std::set<std::string> tmp;
            while (std::getline(ss, item, ',')) {
                tmp.insert(item);
            }
            for (auto& e: tmp){
                if (!prop_val[idx].count(e)){
                    prop_val[idx].insert(e);
                }
            }
            std::string nackpp = std::to_string(msgSpid);
            if (!nack[idx].count(nackpp))
                nack[idx].insert(nackpp);
            if (!finished[idx] && static_cast<int>(nack[idx].size()) > 0 && static_cast<int>(nack[idx].size()) + static_cast<int>(ack[idx].size()) >= N / 2){
                active_prop_num[idx] += 1;
                ack[idx].clear();
                nack[idx].clear();
                std::string nbm = "p" + std::to_string(idx) + "-" + std::to_string(active_prop_num[idx]) + "[";
                for (auto& pv : prop_val[idx]){
                    nbm += pv + ",";
                }
                accepted_val[idx] = prop_val[idx];
                //std::cout << pid << " nack trigger proposed " << nbm << "\n";
                beb->broadcast(nbm);
            }
        }
    }

}