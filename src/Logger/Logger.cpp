#include "Logger.h"

std::ostream & operator<<(std::ostream & stream, const Severity & rhs) {
    switch(rhs) {
        case Debug:
            return stream << std::string("Debug");
        case Info:
            return stream << std::string("Info");
        case Warning:
            return stream << std::string("Warning");
        case Critical:
            return stream << std::string("Critical");
        default:
            return stream << std::string("(unknown)");
    }
}
