#pragma once

#include <string>
#include <vector>
#include <list>
#include <stdexcept>

#include "../Utility.cpp"

/// Delta (difference) kind, whether a chunk
/// was added, deleted, or modified
/// on direction source file -> target file.
enum DataDeltaKind {
    Addition,
    Deletion,
    Modification
};

/// Some differs might require specific output
/// to make sense to user. This is used to achieve that.
/// One can set default for the default behaviour, or force
/// a specific one.
enum ForcedOutput {
    Default,
    DirectPrint,
};

/// Write the delta kind (addition/deletion/modification) to target stream,
/// in string format (that is, it's enum name).
///
/// \param stream Output stream
/// \param delta Delta kind
/// \return Enum name of delta kind
std::ostream & operator<<(std::ostream & stream, const DataDeltaKind & delta);

/// Context of modification (see DataDeltaKind).
///
/// Stores raw context data (that is, usually lines of
/// source file), and two indexes, describing which ranges
/// of the data correspond to what.
struct DataContext {
    /// Raw context data, in terms of lines of chunks
    std::vector<std::string> rawData;
    /// Maximum index of before-modification-context.
    /// Thus, before-modification context has index <0; maxIdxOfBeforeContext).
    size_t maxIdxOfBeforeContext;
    /// Maximum index of on-modification context.
    /// Thus, on-modification context has index <maxIdxOfBeforeContext; maxIdxOfOnContext).
    /// The rest of it is after-modification context.
    size_t maxIdxOfOnContext;

    /// Merge current context with next one. Ignore modifications,
    /// but merge adjacent additions and deletions, and merge them into
    /// one big blob.
    ///
    /// \param rhs Other context to merge with.
    /// \return Reference to itself, so merging can be chained.
    DataContext & mergeWithNewContext(const DataContext & rhs);
};

/// One chunk change
struct DataDelta {
    DataDelta(DataDeltaKind kind, std::vector<std::string> deltaInfo, DataContext sourceFileContext,
              size_t indicatorSource, size_t indicatorDestination, size_t indicatorSourceLen, size_t indicatorDestLen);

    DataDeltaKind kind;
    /// This contains change information about the delta, most often the diff itself.
    std::vector<std::string> deltaInfo;
    /// This contains context from the source file.
    DataContext sourceFileContext;
    /// Line (or byte, depends on the context) of the source file, that is affected by this change.
    size_t indicatorSource;
    /// Line (or byte, depends on the context) of the destination file, that is affected by this change.
    size_t indicatorDestination;
    /// Length of change in source file
    size_t indicatorSourceLen;
    /// Length of change in destination file
    size_t indicatorDestLen;
};

/// Collection of chunk changes,
/// represent whole set of differences between
/// two files.
struct DataDifference {
    /// Source filename
    std::string sourceName;
    /// Destination filename
    std::string destinationName;
    /// List of chunk changes
    std::list<DataDelta> deltas;
    /// If specific output style should be used,
    /// it will be recorded with this enum.
    ForcedOutput forcedOutputStyle;

    DataDifference(std::string sourceName, std::string destinationName,
                   std::list<DataDelta> deltas, ForcedOutput forcedOutputStyle);

/// Normalize data difference - that is, merge adjacent
    /// additions, deletions and replacements.
    void Normalize();
};
