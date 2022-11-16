#if !defined(fifoRb)
#define fifoRb

#include "urb.h"
#include "util.h"

class Fiforb
{
private:
    std::atomic<bool> active;
    int numM;
    int numP;
    int pid;
    std::vector<int> targetPids;
    Urb* urb;
    Process* p;

    std::set<std::string> pending;
    int lsn;
    std::vector<int> next;

public:
    Fiforb(int _pid, std::vector<int> _targetPids, Urb* _urb, Process* _p);
    ~Fiforb();

    void start();
    void stop();

    void addMsg(unsigned long num);
    void broadcast(const std::string& msg);
    void deliverLower(const std::string & msg);
};



#endif //fifoRb