#if !defined(process)
#define process

#include <atomic>

#include "util.h"

class Process
{
private:
    int pid;
    std::atomic<bool> active;
    SafeList log;


public:
    Process(int _pid);
    ~Process();
    
    void stop();

    void addLog(const std::string& msg);
    std::list<std::string> getLogs() const;
};



#endif //process