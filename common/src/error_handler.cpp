#include "error_handler.h"
#include <iostream>
#include <cstring>

common::ErrorHandler &common::ErrorHandler::getInstance()
{
    static ErrorHandler instance;
    return instance;
}

// the way of handling errors will be changed - for example logging to a file
void common::ErrorHandler::fatalError(const char *msg, int line_number, const char *file)
{
    std::cerr << "fatal error: " << msg << " (file " << file << ", line "
              << line_number << "), errno: " << strerror(errno) << "\n";
    std::terminate();
}

void common::ErrorHandler::error(const char *msg, int line_number, const char *file)
{
    std::cerr << "error: " << msg << " (file " << file << ", line "
              << line_number << "), errno: " << strerror(errno) << "\n";
}

common::ErrorHandler::ErrorHandler()
{

}
