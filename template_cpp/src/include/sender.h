#if !defined(sender)
#define sender

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <string>
#include <vector>

#define MAX_LENGTH 512

class Sender
{
private:
    std::atomic<bool> active;
    int size;
    std::vector<int> tSocketFds;
    std::vector<sockaddr_in> tSocketAddrs;

public:
    Sender(std::vector<sockaddr_in> tAddrs);
    ~Sender();

    ssize_t send(const std::string & m, int id);

    void start();
    void stop();
};

#endif // sender
