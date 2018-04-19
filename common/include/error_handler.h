#pragma once

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
