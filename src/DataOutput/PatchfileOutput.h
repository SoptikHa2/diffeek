#pragma once

#include "DataOutput.h"

/// Use this for machine-friendly output. Produces patchfile, that can be processed by the `patch(1)` utility.
class PatchfileOutput : public DataOutput {
public:
    explicit PatchfileOutput(Logger & logger);
    bool Write(const DataDifference & difference) override;
};
