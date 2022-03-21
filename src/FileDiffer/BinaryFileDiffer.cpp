#include "BinaryFileDiffer.h"

BinaryFileDiffer::BinaryFileDiffer(const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger) : FileDiffer(mode, logger, precision, filename) { }

std::optional<DataDifference> BinaryFileDiffer::Diff(const FileDiffer &other) const {
    const auto * binaryRhs = dynamic_cast<const BinaryFileDiffer *>(&other);
    if (binaryRhs == nullptr) {
        // TODO: downcast `other` to binary?
        logger.Log("Cannot compare binary data type (left) with non-binary data type (right). Aborting.", Severity::Critical);
        return {};
    }

    LevenshteinMatrix matrix = constructLevenshteinMatrix(data, binaryRhs->data);

    DataDifference result = levenshteinMatrixToDeltas<char>(matrix, data, binaryRhs->data, binaryRhs->filename,
                                                      [this](size_t idx) -> std::string { return Utility::numberToHexString(
                                                              data[idx]); },
                                                      [binaryRhs](size_t idx) -> std::string { return Utility::numberToHexString(
                                                              binaryRhs->data[idx]); });

    return result;
}

bool BinaryFileDiffer::LoadData() {
    inputStream = std::ifstream(filename, std::ios::binary);

    if(!inputStream) {
        logger.Log("Failed to read file \"" + filename + "\", make sure the file exists and is accessible.", Severity::Critical);
        return false;
    }

    data = std::vector<char>((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

    if (data.empty()) {
        logger.Log("No bytes were read from \"" + filename + "\", is the file empty?", Severity::Warning);
    }
    logger.Log("File \"" + filename + "\" loaded successfully. Loaded total of " + std::to_string(data.size()) + " bytes.", Severity::Debug);
    return true;
}

DataContext BinaryFileDiffer::generateContext(size_t offset) const {
    const size_t context_before_len = 8;
    const size_t context_after_len = 8;

    DataContext result;

    std::vector<char> bytes = Utility::vectorSlice(data, (offset - context_before_len > offset) ? 0 : (offset - context_before_len), offset + context_after_len);
    std::vector<std::string> strContext;
    std::transform(bytes.begin(), bytes.end(), back_inserter(strContext), [](char chr){return Utility::numberToHexString<char>(
            chr);});
    result.rawData = strContext;

    size_t lastBeforeIndexIncl = std::min((offset < context_after_len) ? offset : context_before_len, offset);
    result.maxIdxOfBeforeContext = lastBeforeIndexIncl;
    result.maxIdxOfOnContext = result.maxIdxOfBeforeContext + 1; // Assume one line changed

    return result;
}
