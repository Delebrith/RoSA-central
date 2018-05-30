// author: Tomasz Nowak

#pragma once
#include <exception>
#include <string>

namespace common
{
// There are three types of errors:
// - critical - use Exception, handling it usually ends in killing the program
// - non-critical which can be handled in some way - use ExceptionInfo
// - non-critical which cannot be handled (only warning is logged) - use ExceptionInfo::warning
class Exception : public std::exception
{
public:
    Exception(const std::string &message);
    virtual const char *what() const noexcept;
protected:
    std::string msg;
};

class ExceptionInfo : public Exception
{
public:
    ExceptionInfo(const std::string &message);
    static void warning(const std::string &message); // prints warning message, without throwing exception
};
}
