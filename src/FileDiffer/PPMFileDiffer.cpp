#include "PPMFileDiffer.h"

PPMFileDiffer::PPMFileDiffer(const std::string &filename, const LoadMode &mode, Logger &logger) :
    FileDiffer(mode, logger, {}, filename) {}

std::optional<DataDifference> PPMFileDiffer::Diff(const FileDiffer &other) const {
    if (!file) return {};

    const auto *ppmRhs = dynamic_cast<const PPMFileDiffer *>(&other);
    if (ppmRhs == nullptr) {
        logger.Log("Cannot compare ppm data type (left) with non-ppm data type (right). Aborting.", Severity::Critical);
        return {};
    }

    file->XORWith(*(ppmRhs->file));

    std::ostringstream output;
    output << *file;

    return DataDifference(
            filename,
            ppmRhs->filename,
            {
                    DataDelta(
                            DataDeltaKind::Modification,
                            {output.str()},
                            {},
                            0, 0, 0, 0
                    )
            },
            ForcedOutput::DirectPrint
    );
}

bool PPMFileDiffer::LoadData() {
    auto loaded = PPMFile::Load(filename, logger);
    if (!loaded.has_value()) return false;
    file = std::make_unique<PPMFile>(std::move(*loaded)); // TODO: Is this memory safe?
    return true;
}

DataContext PPMFileDiffer::generateContext(size_t offset) const {
    return DataContext();
}
