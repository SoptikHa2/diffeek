#pragma once

#include "DataOutput.h"
#include "../Utility.cpp"

/// Use this for user-friendly output. This uses 8-bit ANSI colors.
class GraphicalOutput : public DataOutput {
public:
    explicit GraphicalOutput(Logger & logger);
    bool Write(const DataDifference & difference) override;
};
