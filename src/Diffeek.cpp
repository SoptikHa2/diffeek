#include "Diffeek.h"

Diffeek::Diffeek(const ArgData & argumentData, Logger & logger) {
    // First of all, load file loaders
    if (argumentData.dataFormat != Automatic) {
        fileLoaderFrom = FileDiffer::GetInstance(argumentData.dataFormat, argumentData.firstFilename, argumentData.dataLoadMode, argumentData.precision, logger);
        fileLoaderTo = FileDiffer::GetInstance(argumentData.dataFormat, argumentData.secondFilename, argumentData.dataLoadMode, argumentData.precision, logger);
    } else {
        fileLoaderFrom = FileDiffer::GetInstance(argumentData.firstFilename, argumentData.dataLoadMode, argumentData.precision, logger);
        fileLoaderTo = FileDiffer::GetInstance(argumentData.secondFilename, argumentData.dataLoadMode, argumentData.precision, logger);
    }

    loadMode = argumentData.dataLoadMode;
}

bool Diffeek::LoadData(Logger & logger) {
    if(!fileLoaderFrom->LoadData()) return false;
    if(!fileLoaderTo->LoadData()) return false;
    logger.Log("Files loaded.", Severity::Debug);
    return true;
}

std::optional<DataDifference> Diffeek::Diff() const {
    auto result = fileLoaderFrom->Diff(*fileLoaderTo);
    if (result.has_value()) {
        result->Normalize();
    }
    return result;
}
