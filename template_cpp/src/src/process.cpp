#include "process.h"

#include <iostream>

Process::Process(int _pid){
    pid = _pid;
    active = true;
}

Process::~Process(){}

void Process::stop(){
    active = false;
}

void Process::addLog(const std::string& msg){
    if (log.contains(msg))
        return;
    log.push_back(msg);
    //std::cout << pid << " add log " << msg << "\n";
}

std::list<std::string> Process::getLogs() const{
    if (active){
        std::cerr << "main tried to getLog of active process " << pid << "\n";
        return std::list<std::string> ();
    }
    return log.get();
}