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
    int s;
    mutable std::mutex _listMutex;

public:
    SafeList();
    SafeList(const SafeList & sl);
    ~SafeList();

    bool contains(const std::string &item) const;

    void push_back(const std::string &item);

    std::list<std::string> get() const;

    int getSize() const;
};


#endif // util