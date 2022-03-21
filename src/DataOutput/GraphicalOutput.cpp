#include "GraphicalOutput.h"

bool GraphicalOutput::Write(const DataDifference &difference) {
    using namespace Utility::Colors;

    std::cout << "Difference between files \"" << difference.sourceName << "\" -> \"" << difference.destinationName << "\":" << std::endl;
    if (difference.deltas.empty()) {
        std::cout << "Files are identical." << std::endl;
        return true;
    }
    for (const auto & delta : difference.deltas) {
        std::cout << std::endl;

        std::cout << ansi_gray_text << delta.kind << " at offset +" << delta.indicatorSource << ansi_reset << std::endl;

        // Write before-change-context
        for (size_t i = 0; i < delta.sourceFileContext.maxIdxOfBeforeContext; i++) {
                if (i >= delta.sourceFileContext.rawData.size()) break;
                std::cout << delta.sourceFileContext.rawData[i] << std::endl;
        }

        switch(delta.kind) {
            case Addition:
                for (const auto & deltaInfo : delta.deltaInfo) {
                    if (deltaInfo.empty()) std::cout << ansi_green << ansi_gray_text << "(empty line)" << ansi_reset << std::endl;
                    else std::cout << ansi_green << deltaInfo << ansi_reset << std::endl;
                }
                break;
            case Deletion:
                for (const auto & deltaInfo : delta.deltaInfo) {
                    if (deltaInfo.empty()) std::cout << ansi_red << ansi_gray_text << "(empty line)" << ansi_reset << std::endl;
                    else std::cout << ansi_red << deltaInfo << ansi_reset << std::endl;
                }
                break;
            case Modification: {
                size_t k = delta.sourceFileContext.maxIdxOfBeforeContext;
                for (const auto &deltaInfo : delta.deltaInfo) {
                    if (k < delta.sourceFileContext.rawData.size())
                        std::cout << ansi_red << delta.sourceFileContext.rawData[k] << ansi_reset << std::endl;
                    std::cout << ansi_green << deltaInfo << ansi_reset << std::endl;
                    k++;
                }
                break;
            }
            default:
                throw std::logic_error("Missing delta case: graphical output");
        }

        // Write after-change-context
        for (size_t i = delta.sourceFileContext.maxIdxOfOnContext; i < delta.sourceFileContext.rawData.size(); i++) {
            if (i >= delta.sourceFileContext.rawData.size()) break;
            std::cout << delta.sourceFileContext.rawData[i] << std::endl;
        }
    }

    return std::cout.good();
}

GraphicalOutput::GraphicalOutput(Logger &logger) : DataOutput(logger) { }
