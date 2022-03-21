#pragma once

#include <string>
#include <memory>

#include "FileDiffer/FileDiffer.h"
#include "DataOutput/DataDifference.h"
#include "ArgParser.h"

/// The diffing application.
///
/// Provides interface to access other classes, such as
/// file loading and physically data diffing.
class Diffeek {
    std::unique_ptr<FileDiffer> fileLoaderFrom;
    std::unique_ptr<FileDiffer> fileLoaderTo;
    LoadMode loadMode;

public:
    /// Initialize new Diffeek instance with given settings and logger.
    ///
    /// \param argumentData Settings passed as program arguments
    /// \param logger Structure used to log events during runtime
    explicit Diffeek(const ArgData & argumentData, Logger & logger);
    /// Attempt to load data from filesystem into inner structure.
    ////
    /// \param logger Structure used to log events during runtime
    /// \returns A boolean value indicating success. Details
    /// about the run will be logged into logger instance.
    bool LoadData(Logger & logger);
    /// Diff loaded files, as passed in constructor's ArgData.
    ///
    /// \returns A structure with details about the diff result.
    /// The result might be empty, if diffing failed. Details
    /// about the operation will be logged into the logger instance,
    /// that was passed in the constructor.
    std::optional<DataDifference> Diff() const;
};
