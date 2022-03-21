#pragma once

#include <string>

enum Severity {
    Debug,
    Info,
    Warning,
    Critical
};
std::ostream & operator<<(std::ostream & stream, const Severity & rhs);

/// Logging class, that is used to display and store information about inner process of the program.
class Logger {
protected:
    Severity lowestSeverityToDisplay;

    explicit Logger(Severity lowestSeverityToDisplay) : lowestSeverityToDisplay(lowestSeverityToDisplay) {}

public:
    virtual ~Logger() = default;

    /// Write or store information with given severity. This call might be ignored,
    /// if target severity is lower than is setup by user to filter.
    ///
    /// \param text String to write or store
    /// \param severity Severity that determines priority of the log, and whether
    /// it will or won't be ignored.
    virtual void Log(const std::string & text, const Severity & severity) = 0;
    /// Change logger severity settings, typically impacting filtering.
    ///
    /// \param targetSeverity Target severity of the logger to filter.
    /// Lower severities might not be displayed or otherwise recorded.
    virtual void ChangeSeverity(const Severity & targetSeverity) = 0;
};
