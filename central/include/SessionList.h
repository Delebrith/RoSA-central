//
// Created by delebrith on 27.05.18.
//

#ifndef ROSA_CENTRAL_SESSION_LIST_H
#define ROSA_CENTRAL_SESSION_LIST_H

#include <mutex>
#include <set>


class SessionList
{
private:
    std::mutex mutex;
    std::set<std::string> sessions;

public:
    void add_session(std::string address);
    void erase_session(std::string session_id);
    bool exists_session(std::string session_id);
    std::string generate_session_id();
};

#endif //ROSA_CENTRAL_SESSION_LIST_H
