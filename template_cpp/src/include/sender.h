#if !defined(sender)
#define sender

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <string>

#define MAX_LENGTH 512

class Sender
{
private:
    std::atomic<bool> active;
    int tSocketFd;
    struct sockaddr_in tSocketAddr;

public:
    Sender(sockaddr_in tAddr);
    ~Sender();

    ssize_t send(const std::string & m);

    void start();
    void stop();
};

#endif // sender
