#if !defined(bestEffortBroadcast)
#define bestEffortBroadcast

#include "perfectLink.h"

class Beb
{
private:
    std::atomic<bool> active;
    int pid;
    std::vector<int> targetPids;
    PerfectLink * pl;
    Process* p;

public:
    Beb(int _pid, std::vector<int> _targetPids, PerfectLink* _pl, Process* _p);    
    ~Beb();

    void start();
    void stop();

    void addMsg(unsigned long num);
    void broadcast();

    void deliver(const std::string & m);
};

#endif // bestEffortBroadcast