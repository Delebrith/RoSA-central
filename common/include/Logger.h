//
// Created by pszwed
//

#ifndef ROSA_CENTRAL_LOGGER_H
#define ROSA_CENTRAL_LOGGER_H

namespace common {
    class Logger {
        static std::string getTimestamp();

    public:
        static void log(std::string message);
    };
}

#endif //ROSA_CENTRAL_LOGGER_H
