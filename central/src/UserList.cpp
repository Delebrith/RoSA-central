
#include <sys/user.h>
#include <boost/algorithm/string.hpp>
#include "UserList.h"

void UserList::add_user(std::string name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (name.size() > 25)
        throw std::invalid_argument("user name: " + name + " is too long");
    users.emplace_front(name);
}

void UserList::erase_user(std::string name) {
    std::lock_guard<std::mutex> lock(mutex);
    std::list<std::string>::iterator it;
    for (it = users.begin(); it != users.end(); ++it) {
        if (*it == name) {
            users.erase(it);
            break;
        }
    }
    if (it == users.end())
        throw std::invalid_argument("user " + name + " doesn't exist");

}