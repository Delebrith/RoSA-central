// author: Tomasz Nowak
#include "terminal_lock.h"

// This .cpp file is trivial, but I could not put everything in .h (static mutex must be created here)

std::mutex common::TerminalLock::mutex;

common::TerminalLock::TerminalLock()
    : lock(mutex)
{}
