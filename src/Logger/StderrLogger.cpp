#include "StderrLogger.h"

void StderrLogger::Log(const std::string &text, const Severity & severity) {
    // Ignore log attempts with lower severity than is currently set up.
    if (severity >= lowestSeverityToDisplay)
        std::cerr << "[" << getAnsiColorFromSeverity(severity) << severity << Utility::Colors::ansi_reset << "] " << text << std::endl;
}

void StderrLogger::ChangeSeverity(const Severity &targetSeverity) {
    lowestSeverityToDisplay = targetSeverity;
}

const std::string & StderrLogger::getAnsiColorFromSeverity(Severity severity) const {
    switch(severity) {
        case Debug:
            return Utility::Colors::ansi_gray_text;
        case Info:
            return Utility::Colors::ansi_reset;
        case Warning:
            return Utility::Colors::ansi_purple_text;
        case Critical:
            return Utility::Colors::ansi_red_text;
        default:
            throw std::invalid_argument("Unknown severity color");
    }
}
