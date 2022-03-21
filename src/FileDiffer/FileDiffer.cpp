#include "FileDiffer.h"
#include "BinaryFileDiffer.h"
#include "TextFileDiffer.h"
#include "PPMFileDiffer.h"
#include "DirectoryDiffer.h"

bool stringEndsWith (const std::string & str, const std::string & searchTerm) {
    if (str.size() < searchTerm.size()) return false;
    for (size_t i = str.size(), j = searchTerm.size(); i > 0 && j > 0; --i, --j) if (str[i-1] != searchTerm[j-1]) return false;
    return true;
}

const std::string directoryFileExtensions[] = { "/" };
const std::string PPMFileExtensions[] = { ".pbm", ".pgm", ".ppm" };
const std::string textFileExtensions[] = { ".txt", ".md", ".latex", ".tex", ".cpp", ".tpp", ".h", ".rs", ".html", ".xml", ".json" };
const std::string binaryFileExtensions[] = { ".out", ".bin" };


std::unique_ptr<FileDiffer> FileDiffer::GetInstance(const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger) {
    for (const auto & extension : PPMFileExtensions) {
        if (stringEndsWith(filename, extension))
            return std::make_unique<PPMFileDiffer>(filename, mode, logger);
    }
    for (const auto & extension : textFileExtensions) {
        if (stringEndsWith(filename, extension))
            return std::make_unique<TextFileDiffer>(filename, mode, precision, logger);
    }
    for (const auto & extension : binaryFileExtensions) {
        if (stringEndsWith(filename, extension))
            return std::make_unique<BinaryFileDiffer>(filename, mode, precision, logger);
    }
    for (const auto & extension : directoryFileExtensions) {
        if (stringEndsWith(filename, extension))
            return std::make_unique<DirectoryDiffer>(filename, mode, precision, logger);
    }

    // Check if it is a directory
    if (std::filesystem::is_directory(filename)) {
        logger.Log("Determined file \"" + filename + "\" to be a directory.", Severity::Info);
        return std::make_unique<DirectoryDiffer>(filename, mode, precision, logger);
    }

    // If we are dealing with stdin, do not read, and compare via text loudly
    if (filename == "/dev/stdin") {
        logger.Log("No format for stdin was set, and we cannot determine it without ruining the stream. Falling back to "
                   "purely textual comparsion. Please specify format explicitly.", Severity::Warning);
        return std::make_unique<TextFileDiffer>(filename, mode, precision, logger);
    }

    // Read first 1024 bytes and decide, whether it is binary or text file.
    std::ifstream srcStream(filename, std::ios::in | std::ios::binary);

    unsigned char firstChar;
    for (int byteCount = 0; byteCount < 1024; ++byteCount) {
        unsigned char byteRead = srcStream.get();

        if(!srcStream) break;
        if (byteCount == 0) firstChar = byteRead;
        if (byteCount == 1 && firstChar == 'P' && (byteRead >= '0' && byteRead <= '3')) {
            // Detected P1,2,3 header!
            logger.Log("Determined file \"" + filename + "\" to be PPM image, because of it starting with P1/2/3 header.", Severity::Info);
            return std::make_unique<PPMFileDiffer>(filename, mode, logger);
        }

        // It is determined to be binary file,
        // if it contains byte under 0x20 that isn't CR (0x0A), LF (0x0D), or TAB (0x09)
        // We can't tell otherwise (b/c unicode)
        if (byteRead < 0x20 && byteRead != 0x0A && byteRead != 0x0D && byteRead != 0x09) {
            logger.Log("Determined file \"" + filename + "\" to be binary, because of byte \" + Utility::byteToHexString(byteRead) + \" at index " + std::to_string(byteCount) + ".", Severity::Info);
            return std::make_unique<BinaryFileDiffer>(filename, mode, precision, logger);
        }
    }

    return std::make_unique<TextFileDiffer>(filename, mode, precision, logger);
}

std::unique_ptr<FileDiffer> FileDiffer::GetInstance(DataFormat forcedFormat, const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger) {
    if (forcedFormat == DataFormat::Automatic) return FileDiffer::GetInstance(filename, mode, precision, logger);
    switch(forcedFormat) {
       case Binary:
           return std::make_unique<BinaryFileDiffer>(filename, mode, precision, logger);
       case Text:
           return std::make_unique<TextFileDiffer>(filename, mode, precision, logger);
       case PPM:
           return std::make_unique<PPMFileDiffer>(filename, mode, logger);
       case Directory:
           return std::make_unique<DirectoryDiffer>(filename, mode, precision, logger);
        default:
            throw std::invalid_argument("Unknown data format while constructing file loader.");
    }
}

LoadMode operator| (const LoadMode & lhs, const LoadMode & rhs) {
    return (LoadMode)((int)lhs | (int)rhs);
}
LoadMode & operator|= (LoadMode & lhs, const LoadMode & rhs) {
    lhs = lhs | rhs;
    return lhs;
}

LoadMode operator&(const LoadMode &lhs, const LoadMode &rhs) {
    return (LoadMode)((int)lhs & (int)rhs);
}
