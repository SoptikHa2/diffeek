#pragma once

#include <string>
#include <optional>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "DataDifference.h"
#include "../ArgParser.h"
#include "../Logger/Logger.h"
#include "../Utility.cpp"

/// Generic class that displays set of differences between files
class DataOutput {
protected:
    Logger & logger;
    explicit DataOutput(Logger & logger);

public:
    virtual ~DataOutput() = default;

    /// Considering user settings (argument data), environment context (such as output type), and settings by
    /// differ that generated the difference, choose, create and return an instance of one chosen DataOutput subclass.
    ///
    /// \param data Argument data
    /// \param difference Set of differences from source to destination file
    /// \param logger Logger
    /// \return Unique pointer to instance of a DataOutput subclass
    static std::unique_ptr<DataOutput> GetOutput(const ArgData & data, const DataDifference & difference, Logger & logger);

    /// Print data difference to an output channel. Specific output channel is set up upon instantination of the class.
    ///
    /// \param difference Set of differences from source to destination file
    /// \return Whether the write to target file was successful.
    virtual bool Write(const DataDifference & difference) = 0;
};
