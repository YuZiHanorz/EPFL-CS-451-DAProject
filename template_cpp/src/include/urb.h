#if !defined(uniformReliabletBroadcast)
#define uniformReliabletBroadcast

#include "beb.h"
#include "util.h"
#include <unordered_map>
#include <set>

class Fiforb;

class Urb
{
private:
    std::atomic<bool> active;
    int numM;
    int pid;
    std::vector<int> targetPids;
    Beb* beb;
    Process* p;
    Fiforb* fb;

    std::set<std::string> delivered;
    std::set<std::string> pending;
    std::unordered_map<std::string, std::set<std::string>> ack;

    std::thread deliverThread;
    std::mutex ackMutex, penMutex;

public:
    Urb(int _pid, std::vector<int> _targetPids, Beb* _beb, Process* _p);
    ~Urb();

    void setUpper(Fiforb* _fb);
    void start();
    void stop();

    void addMsg(unsigned long num);
    void broadcast(const std::string& msg);
    void deliverLower(const std::string & msg);

    bool canDeliver(const std::string & sm);

    void deliverT();
};



#endif //uniformReliabletBroadcast