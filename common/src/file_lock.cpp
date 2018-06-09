// created by p.szwed

#include "file_lock.h"

std::mutex common::FileLock::mutex;

common::FileLock::FileLock()
        : lock(mutex)
{}
