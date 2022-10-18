#include "util.h"

#include <algorithm>

SafeList::SafeList(){}

SafeList::~SafeList(){}

bool SafeList::contains(const std::string &str) const {
    std::lock_guard<std::mutex> lock(_listMutex);

    std::list<std::string>::const_iterator it = find(_list.begin(), _list.end(), str);
    return it != _list.end();
}

void SafeList::push_back(const std::string &str) {
    std::lock_guard<std::mutex> lock(_listMutex);

    _list.push_back(str);
}

std::list<std::string> SafeList::get() const {
    std::lock_guard<std::mutex> lock(_listMutex);

    return _list;
}
