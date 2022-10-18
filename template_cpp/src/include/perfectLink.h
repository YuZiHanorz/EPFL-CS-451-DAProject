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

class PerfectLink
{
private:
    std::atomic<bool> active;
    int pid;
    int targetPid;
    Receiver* rec;
    Sender* sen;
    Process* p;

    std::list<std::string> pendingMsgs, pendingAcks;

    std::mutex logMutex, msgMutex, ackMutex;

    std::thread recThread;
    std::thread sendThread;
    std::thread ackThread;

public:
    PerfectLink(int _pid, int _targetPid, Receiver* _rec, Sender* _sen, Process* _p);
    ~PerfectLink();

    void sendT();
    void recT();
    void ackT();

    void start();
    void stop();

    void addMsg(const std::string msg);
    void addMsgs(const std::list<std::string> msgs);
};

#endif // pLink
