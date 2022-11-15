#if !defined(bestEffortBroadcast)
#define bestEffortBroadcast

#include "perfectLink.h"

class Urb;

class Beb
{
private:
    std::atomic<bool> active;
    int pid;
    std::vector<int> targetPids;
    PerfectLink * pl;
    Process* p;
    Urb* urb;

public:
    Beb(int _pid, std::vector<int> _targetPids, PerfectLink* _pl, Process* _p);    
    ~Beb();

    void setUpper(Urb* _urb);
    void start();
    void stop();

    void addMsg(unsigned long num);
    
    void broadcast(const std::string& msg);

    void deliver(const std::string & msg);
};

#endif // bestEffortBroadcast