// author: Tomasz Nowak

#include "exception.h"
#include <iostream>

common::Exception::Exception(const std::string &message)
    : msg(message + "\n")
{}

const char *common::Exception::what() const noexcept
{
    return msg.c_str();
}

common::ExceptionInfo::ExceptionInfo(const std::string &message)
    : common::Exception(message)
{}

void common::ExceptionInfo::warning(const std::string &message)
{
    std::cerr << "warning: " << message << "\n";
}
