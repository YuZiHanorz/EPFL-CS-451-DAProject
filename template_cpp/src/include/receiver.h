#if !defined(receiver)
#define receiver

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <string>
#include <list>

#include "util.h"

#define MAX_LENGTH 1024

class Receiver
{
private:
    std::atomic<bool> active;
    int socketFd;
    struct sockaddr_in socketAddr;

public:
    Receiver(in_addr_t ip, unsigned short port);
    ~Receiver();

    ssize_t receive(char* buffer);

    void start();
    void stop();
};



#endif // receiver