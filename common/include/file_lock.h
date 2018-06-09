//
// Created by p.szwed
//

#ifndef ROSA_CENTRAL_FILE_LOCK_H
#define ROSA_CENTRAL_FILE_LOCK_H

#include <mutex>

namespace common
{
    class FileLock
    {
    public:
        FileLock();

    private:
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock;
    };
}

#endif //ROSA_CENTRAL_FILE_LOCK_H
