#if !defined(pLink)
#define pLink

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "receiver.h"
#include "sender.h"
#include "process.h"

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <list>

class Beb;

class PerfectLink
{
private:
    std::atomic<bool> active;
    int pid;
    std::vector<int> targetPids;
    Receiver* rec;
    Sender* sen;
    Beb* beb;

    std::list<std::string> pendingMsgs, pendingAcks;

    std::mutex logMutex, msgMutex, ackMutex, linkMutex;

    std::thread recThread;
    std::thread sendThread;
    std::thread ackThread;

    //std::vector<PerfectLink*> others;

public:
    PerfectLink(int _pid, std::vector<int> _targetPids, Receiver* _rec, Sender* _sen);
    ~PerfectLink();

    void sendT();
    void recT();

    void start();
    void stop();

    void addMsg(const std::string& msg);
    void addMsgs(const std::list<std::string>& msgs);
    void setBroadcast(Beb* _beb);
};

#endif // pLink
