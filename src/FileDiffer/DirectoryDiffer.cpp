#include "DirectoryDiffer.h"

DirectoryDiffer::DirectoryDiffer(const std::string & dirname, const LoadMode & mode, std::optional<size_t> precision, Logger & logger) : FileDiffer(mode, logger, precision, dirname), rootEntry(dirname, logger) { }

std::optional<DataDifference> DirectoryDiffer::Diff(const FileDiffer &other) const {
    const auto * directoryRhs = dynamic_cast<const DirectoryDiffer *>(&other);
    if (directoryRhs == nullptr) {
        logger.Log("Cannot compare directory data type (left) with non-directory data type (right). Aborting.", Severity::Critical);
        return {};
    }

    LevenshteinMatrix matrix = constructLevenshteinMatrix(stringifiedViewHashes, directoryRhs->stringifiedViewHashes);

    DataDifference result = levenshteinMatrixToDeltas(matrix, stringifiedViewHashes, directoryRhs->stringifiedViewHashes, directoryRhs->filename,
                                                      [this](size_t idx) -> std::string { return stringifiedView[idx]; },
                                                      [directoryRhs](size_t idx) -> std::string { return directoryRhs->stringifiedView[idx]; });

    return result;
}

bool DirectoryDiffer::LoadData() {
    if (!is_directory(rootEntry.path)) {
        logger.Log("Error: file is not a directory, while trying to load filesystem rooted at file \"" + filename + "\".", Severity::Critical);
        return false;
    }

    try {
        loadDirectory(rootEntry);
    } catch(const std::filesystem::filesystem_error & fserr) {
        logger.Log("A filesystem error occured while trying to compare filesystem \"" + filename + "\": " + fserr.what(), Severity::Critical);
        return false;
    }

    // Now, when everything was loaded, convert it to textual form, so we can easily compare it.
    addToStringifiedView(rootEntry);

    return true;
}

DataContext DirectoryDiffer::generateContext(size_t offset) const {
    const size_t context_before_len = 4;
    const size_t context_after_len = 2;

    DataContext result;

    result.rawData = Utility::vectorSlice(stringifiedView, (offset - context_before_len > offset) ? 0 : (offset - context_before_len), offset + context_after_len);

    size_t lastBeforeIndexIncl = std::min((offset < context_after_len) ? offset : context_before_len, offset);
    result.maxIdxOfBeforeContext = lastBeforeIndexIncl;
    result.maxIdxOfOnContext = result.maxIdxOfBeforeContext + 1; // Assume one line changed

    return result;
}

void DirectoryDiffer::loadDirectory(DirectoryDifferFilesystemEntry & directory) {
    for (const auto & file : std::filesystem::directory_iterator(directory.path)) {
        std::string filename = file.path().filename().generic_string();
        directory.contents[filename] = DirectoryDifferFilesystemEntry(file.path(), logger);
        if (file.is_directory()) loadDirectory(directory.contents.at(filename));
    }
}

void DirectoryDiffer::addToStringifiedView(const DirectoryDifferFilesystemEntry &file) {
    // Add to hashes without indent, that one is for user display only.
    std::ostringstream stream;
    stream << relative(file.path, rootEntry.path).generic_string();
    if (file.displayAsDirectory) {
        stream << "/";
    } else {
        stream << " (" << Utility::numberToHexString(file.hash) << ")";
    }
    std::string fileIdentifier = stream.str();
    Utility::u32 fileIdentifierHash = Utility::Murmur3(fileIdentifier);

    stringifiedView.push_back(stream.str());
    stringifiedViewHashes.push_back(fileIdentifierHash);

    // Print all children, if any
    for(const auto & child : file.contents) {
        addToStringifiedView(child.second);
    }
}

DirectoryDifferFilesystemEntry::DirectoryDifferFilesystemEntry(std::filesystem::path path, Logger & logger) : path(std::move(path)) {
    displayAsDirectory = std::filesystem::is_directory(this->path);

    // If the file is a regular file, compute hash
    if (std::filesystem::is_regular_file(this->path)) {
        size_t size = std::filesystem::file_size(this->path);
        std::ifstream inputStream(this->path, std::ios::binary);

        if (!inputStream) {
            logger.Log("Failed to read file \"" + this->path.generic_string() + "\", make sure the file exists and is accessible.", Severity::Warning);
            return;
        }

        size_t sizeToRead = Utility::min<size_t>(size, 4096);

        char buffer[4096];
        inputStream.read(buffer, sizeToRead);

        if (inputStream.fail()) {
            logger.Log("Failure after reading from file \"" + this->path.generic_string() + "\".", Severity::Warning);
            return;
        }

        std::vector<char> bytes(buffer, buffer+sizeToRead);
        hash = Utility::Murmur3(bytes);
    }
}
