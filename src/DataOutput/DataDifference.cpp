#include "DataDifference.h"

void DataDifference::Normalize() {
    // Merge subsequent deltas
    std::list<DataDelta> newDeltas;

    for (const auto &delta : deltas) {
        if (newDeltas.empty()) {
            newDeltas.push_back(delta);
            continue;
        }

        // Check, if it has the same type which is not modification and is right behind previous delta
        if (delta.kind != DataDeltaKind::Modification && delta.kind == newDeltas.back().kind &&
            newDeltas.back().indicatorSource + newDeltas.back().indicatorSourceLen == delta.indicatorSource) {
            // Merge the deltas
            newDeltas.back().indicatorSourceLen += delta.indicatorSourceLen;
            newDeltas.back().indicatorDestLen += delta.indicatorDestLen;
            // Merge context - keep only beginning and ending context of the old and new delta.
            newDeltas.back().sourceFileContext.mergeWithNewContext(delta.sourceFileContext);
            // Copy the change itself
            std::copy(delta.deltaInfo.begin(), delta.deltaInfo.end(), back_inserter(newDeltas.back().deltaInfo));
        } else {
            newDeltas.push_back(delta);
        }
    }

    deltas = newDeltas;
}

DataDifference::DataDifference(std::string sourceName, std::string destinationName,
                               std::list<DataDelta> deltas, ForcedOutput forcedOutputStyle) :
        sourceName(std::move(sourceName)), destinationName(std::move(destinationName)), deltas(std::move(deltas)),
        forcedOutputStyle(forcedOutputStyle) {}

std::ostream &operator<<(std::ostream &stream, const DataDeltaKind &delta) {
    switch (delta) {
        case Addition:
            stream << std::string("Addition");
            return stream;
        case Deletion:
            stream << std::string("Deletion");
            return stream;
        case Modification:
            stream << std::string("Modification");
            return stream;
        default:
            throw std::logic_error("Missing delta case: << operator");
    }
}

DataContext &DataContext::mergeWithNewContext(const DataContext &rhs) {
    // Throw away old after-context
    std::vector<std::string> newContext = Utility::vectorSlice(rawData, 0, maxIdxOfOnContext);
    // Copy new on- and after- context
    std::copy(rhs.rawData.begin() + rhs.maxIdxOfOnContext - 1, rhs.rawData.end(), back_inserter(newContext));

    maxIdxOfOnContext = maxIdxOfBeforeContext + (maxIdxOfOnContext - maxIdxOfBeforeContext) // context offset from this
                        + (rhs.maxIdxOfOnContext - rhs.maxIdxOfBeforeContext); // context offset from rhs
    rawData = newContext;
    return *this;
}

DataDelta::DataDelta(DataDeltaKind kind, std::vector<std::string> deltaInfo,
                     DataContext sourceFileContext, size_t indicatorSource, size_t indicatorDestination,
                     size_t indicatorSourceLen, size_t indicatorDestLen) : kind(kind), deltaInfo(std::move(deltaInfo)),
                                                                           sourceFileContext(std::move(sourceFileContext)),
                                                                           indicatorSource(indicatorSource),
                                                                           indicatorDestination(indicatorDestination),
                                                                           indicatorSourceLen(indicatorSourceLen),
                                                                           indicatorDestLen(indicatorDestLen) {}
