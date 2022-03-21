#pragma once

#include <string>

#include "FileDiffer.h"
#include "DataStructures/PPMFile.h"

/// This loads PPM file
class PPMFileDiffer : public FileDiffer {
    std::unique_ptr<PPMFile> file;

public:
    PPMFileDiffer(const std::string & filename, const LoadMode & mode, Logger & logger);
    bool LoadData() override;
    std::optional<DataDifference> Diff(const FileDiffer & other) const override;

private:
    DataContext generateContext(size_t offset) const override;
};
