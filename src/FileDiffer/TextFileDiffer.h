#pragma once

#include <string>
#include <cctype>
#include <algorithm>

#include "FileDiffer.h"

class TextFileDiffer : public FileDiffer {
    typedef u_int32_t u32;

    std::vector<std::string> lines;
    /// Murmur3 hash of each line, used to quickly determine,
    /// whether two lines are equal.
    ///
    /// Regarding hash collisions: we might compare
    /// the line char-by-char on hash hit in order to
    /// prevent false positive matches, but we don't really
    /// need to b/c of Murmur3 properties and because
    /// this is mere file differ.
    std::vector<u32> lineHashes;

public:
    TextFileDiffer(const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger);
    bool LoadData() override;
    std::optional<DataDifference> Diff(const FileDiffer & other) const override;

private:
    std::string normalizeText(std::string text) const;
    DataContext generateContext(size_t offset) const override;
};
