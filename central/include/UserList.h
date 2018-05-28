#ifndef ROSA_CENTRAL_USERLIST_H
#define ROSA_CENTRAL_USERLIST_H

#include <mutex>
#include <list>
#include <vector>

class UserList {

private:
    std::mutex mutex;
    std::list<std::string> users;
public:
    void add_user(std::string name);

    void erase_user(std::string name);
};


#endif //ROSA_CENTRAL_USERLIST_H
