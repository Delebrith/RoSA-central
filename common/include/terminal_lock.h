//
// Created by T. Nowak
//

#pragma once
#include <mutex>

// Simple class holding global mutex, locks it in constructor and automaticallly unlocks in destructor.
// It is needed to synchronize access to the terminal (standard output) from many threads, 
// however it could also be used in some other way, because it has nothing to do with std streams.
//
// Usage example:
// common::TerminalLock(), std::cout << "write antyhing (" << std::to_string(...) << ") you like before the semicolon" << std::endl;
// 
// In the example above, Temporary TerminalLock is constructed, than something is executed after the comma operator, 
// and finally after executing everything before the semicolon, temporary TerminalLock object is destroyed.
//
// You can also use it the following way - creating TerminalLock that lasts a bit longer in a block:
// {
//     common::TerminalLock lock;
//     do();
//     something();
//     complex();
// }

namespace common
{
class TerminalLock
{
public:
    TerminalLock();

private:
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock;
};
}
