#include "DataOutput.h"
#include "PatchfileOutput.h"
#include "GraphicalOutput.h"
#include "DirectOutput.h"

DataOutput::DataOutput(Logger &logger) : logger(logger) { }

std::unique_ptr<DataOutput> DataOutput::GetOutput(const ArgData & data, const DataDifference & difference, Logger & logger) {
    // If file type requires direct output, use it, no matter what user said
    if (difference.forcedOutputStyle == ForcedOutput::DirectPrint)
        return std::make_unique<DirectOutput>(logger);

    // Some output style was specifically forced, use it, no questions asked.
    if (data.forcedOutputStyle.has_value()) {
        if (*data.forcedOutputStyle == ForcedOutputStyle::Patchfile) {
            return std::make_unique<PatchfileOutput>(logger);
        } else if (*data.forcedOutputStyle == ForcedOutputStyle::Graphical) {
            return std::make_unique<GraphicalOutput>(logger);
        } else {
            throw std::logic_error("Invalid forced output style: " + std::to_string(*data.forcedOutputStyle));
        }
    }

    if(!isatty(1)) {
        // File descriptor 1 does not point to a terminal -> unless forced otherwise,
        // use patch output.
        return std::make_unique<PatchfileOutput>(logger);
    }

    // User wants to print to stdout, and we determined stdout to be a tty.
    // Use graphical output.
    return std::make_unique<GraphicalOutput>(logger);
}
