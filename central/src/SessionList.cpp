//
// Created by p.szwed
//

#include <random>
#include <algorithm>
#include "SessionList.h"

void SessionList::add_session(std::string session_id) {
    std::lock_guard<std::mutex> lock(mutex);
    sessions.emplace(session_id);
}

void SessionList::erase_session(std::string session_id) {
    int i;
    {
        std::lock_guard<std::mutex> lock(mutex);
        i = sessions.erase(session_id);
    }
    if(i == 0)
        throw std::invalid_argument("session with ID" + session_id +"doesn't exist");
}

bool SessionList::exists_session(std::string session_id)
{
    std::set<std::string>::iterator i;
    {
        std::lock_guard <std::mutex> lock(mutex);
        i = sessions.find(session_id);
    }
    if(i == sessions.end())
        throw std::invalid_argument("Session does nnot exist!");
    return true;
}

std::string SessionList::generate_session_id()
{
    int length = 16;
    auto randchar = []() -> char
    {
        const char charset[] =
                "0123456789";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}