#if !defined(lattice)
#define lattice

#include "beb.h"
#include "util.h"
#include <unordered_map>
#include <set>
#include <vector>

class LatticeAgr
{
private:
    std::atomic<bool> active;
    int numM;
    int pid;
    std::vector<int> targetPids;
    Beb* beb;
    Process* p;

    std::vector<bool> finished;
    std::vector<std::set<std::string>> ack;
    std::vector<std::set<std::string>> nack;
    std::vector<int> active_prop_num;
    std::vector<std::set<std::string>> prop_val;
    std::vector<std::set<std::string>> accepted_val;
    

public:
    LatticeAgr(int _pid, std::vector<int> _targetPids, Beb* _beb, Process* _p);
    ~LatticeAgr();

    void start();
    void stop();

    void propose(const std::vector<std::set<std::string>> & prop);
    void deliverLower(const std::string & msg);
};



#endif //lattice