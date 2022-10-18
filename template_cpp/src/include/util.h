#if !defined(util)
#define util

#include <list>
#include <mutex>
#include <string>
#include <atomic>

class SafeList
{
private:
    std::list<std::string> _list;
    mutable std::mutex _listMutex;

public:
    SafeList();
    ~SafeList();

    bool contains(const std::string &item) const;

    void push_back(const std::string &item);

    std::list<std::string> get() const;
};


#endif // util