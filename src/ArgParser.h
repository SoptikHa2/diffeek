#pragma once

#include <optional>
#include <string>

#include "FileDiffer/FileDiffer.h"
#include "Logger/Logger.h"

struct ArgData;

/// Argument parser class, takes care of argv.
class ArgParser {
    int argc;
    char ** argv;

public:
    /// Construct ArgParser with specified arguments passed
    /// to the program.
    ///
    /// \param argc Argument count, as received from the environment.
    /// \param argv Array of cstrings, representing accepted arguments,
    /// as received from the environment. Individual cstrings must
    /// be null-terminated.
    ArgParser(int argc, char ** argv);
    /// Parse arguments received in constructor and,
    /// if successful, return parsed ArgData structure.
    ///
    /// \param logger Logger instance used to log all interesting
    /// events that happen during parsing.
    /// \return ArgData, containing parsed argument data, if
    /// successful. Details will be sent to the logger instance.
    std::optional<ArgData> parse(Logger & logger) const noexcept;
};

/// User might specify program-readable (patchfile) or
/// user-readable (graphical) output style.
/// This enum is used to represent the choice.
enum ForcedOutputStyle {
    Patchfile,
    Graphical,
};

/// Structure containing parsed argument data from user.
/// Once this structure is created, the data are
/// guaranteed to be valid.
struct ArgData {
    /// Path to the first filename to diff, the source.
    std::string firstFilename;
    /// Path to the second filename to diff, the destination.
    std::string secondFilename;
    /// User might request a specific data format to be used
    /// during diffing.
    DataFormat dataFormat = DataFormat::Automatic;
    /// User might request a specific diffing (also 'loading') mode
    /// that will, for example, skip whitespaces or ignore case
    /// while text-diffing.
    LoadMode dataLoadMode = LoadMode::Permissive;
    /// Usually associated with -h or --help, this specifies
    /// whether we should show help menu to the user
    /// instead of running the program.
    bool showUsage = false;
    /// Forced output format, either human or compute readable.
    /// Might not be specified.
    std::optional<ForcedOutputStyle> forcedOutputStyle;
    std::optional<size_t> precision = {};
    /// Logging verbosity specified by user
    int verbosity = 0;
};
