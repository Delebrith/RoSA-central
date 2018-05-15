#pragma once

#ifdef DEBUG
#define ASSERT(x) \
    if(!(x))  \
        ErrorHandler::getInstance().fatalError("assertion failed: " #x, __LINE__ ,__FILE__);
#else
#define ASSERT(x) (void) x;
#endif

namespace common
{
    class ErrorHandler
    {
    public:
        static ErrorHandler& getInstance(); // singleton
        void fatalError(const char *msg, int line_number, const char *file);
        void error(const char *msg, int line_number, const char *file);

    private:
        ErrorHandler();
    };
}
