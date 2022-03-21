#pragma once

#include <map>
#include <filesystem>
#include <vector>

#include "FileDiffer.h"

/// Represents one file in the filesystem hierarchy.
class DirectoryDifferFilesystemEntry {

public:
    /// Hash of the file
    Utility::u32 hash = 0;
    /// Path to the file
    std::filesystem::path path;
    /// If the file is a directory, map [name -> file] of it's contents
    std::map<std::string, DirectoryDifferFilesystemEntry> contents;
    /// Whether the file should be displayed to output stream as a directory.
    bool displayAsDirectory;

    DirectoryDifferFilesystemEntry() = default;
    /// Construct new file instance
    /// \param path Path to the file
    /// \param logger Logger
    DirectoryDifferFilesystemEntry(std::filesystem::path path, Logger & logger);
};

class DirectoryDiffer : public FileDiffer {
    /// Entry of the root file, the one specified by user
    DirectoryDifferFilesystemEntry rootEntry;
    /// Part of the filesystem that was analysed, dumped into string for easy comparsion
    std::vector<std::string> stringifiedView;
    /// Hashes of string dump of this part of the filesystem
    std::vector<Utility::u32> stringifiedViewHashes;

public:
    DirectoryDiffer(const std::string & dirname, const LoadMode & mode, std::optional<size_t> precision, Logger & logger);
    bool LoadData() override;
    std::optional<DataDifference> Diff(const FileDiffer & other) const override;

private:
    DataContext generateContext(size_t offset) const override;
    /// Load directory on given path, recursively
    void loadDirectory(DirectoryDifferFilesystemEntry & file);
    /// Write string representation of the file into stringifiedView, recursively.
    void addToStringifiedView(const DirectoryDifferFilesystemEntry & file);
};
