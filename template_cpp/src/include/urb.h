#if !defined(uniformReliabletBroadcast)
#define uniformReliabletBroadcast

#include "beb.h"
#include "util.h"

class Urb
{
private:
    std::atomic<bool> active;
    int numM;
    int pid;
    std::vector<int> targetPids;
    Beb* beb;
    Process* p;

    SafeList delivered;
    SafeList pending;
    std::vector<SafeList> ack;

    std::thread deliverThread;
    std::mutex ackMutex, penMutex;

public:
    Urb(int _pid, std::vector<int> _targetPids, Beb* _beb, Process* _p);
    ~Urb();

    void start();
    void stop();

    void addMsg(unsigned long num);
    void broadcast(const std::string& msg);
    void deliverLower(const std::string & msg);

    bool canDeliver(int idx);

    void deliverT();
};



#endif //uniformReliabletBroadcast