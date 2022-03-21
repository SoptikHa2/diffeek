#include "DirectOutput.h"

DirectOutput::DirectOutput(Logger &logger) : DataOutput(logger) { }

bool DirectOutput::Write(const DataDifference &difference) {
    for (const auto & delta : difference.deltas) {
        for (const auto & line : delta.deltaInfo) {
            std::cout << line;
        }
    }
    return std::cout.good();
}
