#pragma once

#include <vector>

#include "FileDiffer.h"

class BinaryFileDiffer : public FileDiffer {
    std::vector<char> data;

public:
    BinaryFileDiffer(const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger);
    bool LoadData() override;
    std::optional<DataDifference> Diff(const FileDiffer & other) const override;

private:
    DataContext generateContext(size_t offset) const override;
};
