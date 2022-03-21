#pragma once

#include "DataOutput.h"

/// Use this to write changes directly to the stdout, ignore context and everything else.
class DirectOutput : public DataOutput {
public:
    explicit DirectOutput(Logger & logger);
    /// Write changes directly to the stdout, ignore context and everything else.
    bool Write(const DataDifference & difference) override;
};
