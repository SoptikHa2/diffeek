#include "ArgParser.h"

#include <vector>

ArgParser::ArgParser(int argc, char **argv) : argc(argc), argv(argv) { }

std::optional<ArgData> ArgParser::parse(Logger & logger) const noexcept {
    using namespace std;
    ArgData result;

    vector<string> filenames;
    // If user passes `--` flag, we consider everything Afterwards
    // to be a filename, even if it looks like a flag or an option.
    // This variable keeps track of whether we've seen the `--` already.
    bool everything_is_filename = false;

    for (int i = 1; i < argc; ++i) {
        string currentArg (argv[i]);
        // Ignore empty args. This should not happen, but better save than sorry
        if (currentArg.empty()) continue;

        // Treat argument as filename, if it doesn;t start with `-` or
        // we track all options as files.
        if(currentArg[0] != '-' || everything_is_filename || (currentArg[0] == '-' && currentArg.size() == 1)) {
            // Filename
            filenames.push_back(currentArg);
            continue;
        }

        if (currentArg == "--") {
            everything_is_filename = true;
            continue;
        }

        // In this section, parse all the options we provide.

        if (currentArg == "-F") {
            if (i + 1 == argc) {
                logger.Log("Found -F switch without an argument, ignoring.", Severity::Warning);
                continue;
            }
            std::string Farg (argv[++i]);
            std::transform(Farg.begin(), Farg.end(), Farg.begin(), ::tolower);
            if (Farg == "text") result.dataFormat = DataFormat::Text;
            else if (Farg == "binary") result.dataFormat = DataFormat::Binary;
            else if (Farg == "directory" || Farg == "folder") result.dataFormat = DataFormat::Directory;
            else if (Farg == "ppm" || Farg == "pgm" || Farg == "pbm" || Farg == "pnm") result.dataFormat = DataFormat::PPM;
            else logger.Log("Argument of -F switch is unknown, ignoring.", Severity::Warning);
            i++;
            continue;
        }
        if (currentArg == "-x" || currentArg == "--precision") {
            if (i + 1 == argc) {
                logger.Log("Found precision switch without an arugment, ignoring.", Severity::Warning);
                continue;
            }
            try {
                int Xarg = std::stoi(argv[++i]);
                if (Xarg == 0 || Xarg >= 2) result.precision = Xarg;
                else logger.Log("Precision argument out of range {0} u <2; oo), ignoring.", Severity::Warning);
                continue;
            } catch(const std::invalid_argument & excp) {
                logger.Log("Precision argument was not a number, ignoring.", Severity::Warning);
                continue;
            }
        }
        if (currentArg == "-h" || currentArg == "--help") {
            result.showUsage = true;
            return result;
        }
        if (currentArg == "-s" || currentArg == "--strict") {
            // Reset load mode to be considerd strict, strictcase and strictwhitespace - and nothing else.
            result.dataLoadMode = LoadMode::Strict | LoadMode::Text_StrictCase | LoadMode::Text_StrictWhitespace;
            continue;
        }
        if (currentArg == "--text-strictcase") {
            // Append strictcase to load mode
            result.dataLoadMode |= LoadMode::Text_StrictCase;
            continue;
        }
        if (currentArg == "--text-strictwhitespace") {
            // Append strict whitespace to load mode
            result.dataLoadMode |= LoadMode::Text_StrictWhitespace;
            continue;
        }
        if (currentArg == "-p" || currentArg == "--patchfile") {
            // Force output style to machine-readable (patchfile)
            result.forcedOutputStyle = {Patchfile};
            continue;
        }
        if (currentArg == "-g" || currentArg == "--graphical") {
            // Force output style to user-readable (graphical)
            result.forcedOutputStyle = {Graphical};
            continue;
        }
        // if arg starts with -v ...
        if (currentArg.rfind("-v", 0) == 0 || currentArg == "--verbose") {
            result.verbosity++;
            // Check, if -vv, -vvv, etc was specified. Increase verbosity by count of found -v
            size_t vIdx = 2;
            while (currentArg.size() > vIdx && currentArg[vIdx++] == 'v') result.verbosity++;
            continue;
        }

        // Error: failed to match option
        logger.Log("Failed to match option \"" + currentArg + "\" during parsing user-supplied command arguments.", Severity::Critical);
        return {};
    }

    // Check how many filenames were specified.
    if (filenames.size() < 2) {
        logger.Log("Received less than two filenames. Nothing to compare.", Severity::Critical);
        return {};
    }
    if (filenames.size() >= 3) {
        logger.Log("Received too many filenames. Not sure what to do.", Severity::Critical);
        return {};
    }

    // Two filenames are input files, the third one, if specified, is the output file.
    result.firstFilename = filenames[0];
    result.secondFilename = filenames[1];

    if (result.firstFilename == "-") result.firstFilename = "/dev/stdin";
    if (result.secondFilename == "-") result.secondFilename = "/dev/stdin";
    if (result.firstFilename == result.secondFilename && result.firstFilename == "/dev/stdin") {
        // We want to load from stdin twice -> FORBIDDEN
        logger.Log("Attempted to compare stdin to stdin. This is illegal. Aborting.", Severity::Critical);
        return {};
    }

    return result;
}
