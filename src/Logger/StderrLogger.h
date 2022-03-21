#pragma once

#include <iostream>

#include "Logger.h"
#include "../Utility.cpp"

class StderrLogger : public Logger {

public:
    explicit StderrLogger(Severity lowestSeverityToDisplay) : Logger(std::move(lowestSeverityToDisplay)) {}
    void Log(const std::string &text, const Severity & severity) override;
    void ChangeSeverity(const Severity & targetSeverity) override;

private:
    const std::string & getAnsiColorFromSeverity(Severity severity) const;
};
