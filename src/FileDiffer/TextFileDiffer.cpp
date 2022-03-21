#include "TextFileDiffer.h"

TextFileDiffer::TextFileDiffer(const std::string & filename, const LoadMode & mode, std::optional<size_t> precision, Logger & logger) : FileDiffer(mode, logger, precision, filename) { }

std::optional<DataDifference> TextFileDiffer::Diff(const FileDiffer &other) const {
    const auto * textRhs = dynamic_cast<const TextFileDiffer *>(&other);
    if (textRhs == nullptr) {
        // TODO: downcast `other` to text?
        logger.Log("Cannot compare text data type (left) with non-text data type (right). Aborting.", Severity::Critical);
        return {};
    }

    LevenshteinMatrix matrix = constructLevenshteinMatrix(lineHashes, textRhs->lineHashes);

    DataDifference result = levenshteinMatrixToDeltas<u32>(matrix, lineHashes, textRhs->lineHashes, textRhs->filename,
                                                           [this](size_t idx) -> std::string { return lines[idx]; },
                                                           [textRhs](size_t idx) -> std::string { return textRhs->lines[idx]; });

    return result;
}

bool TextFileDiffer::LoadData() {
    inputStream = std::ifstream(filename);

    if(!inputStream) {
        logger.Log("Failed to read file \"" + filename + "\", make sure the file exists and is accessible.", Severity::Critical);
        return false;
    }

    std::string line;
    while(std::getline(inputStream, line)) {
        lines.push_back(line);
        lineHashes.push_back(Utility::Murmur3(normalizeText(line)));
    }

    if (lines.empty()) {
        logger.Log("No lines were read from \"" + filename + "\", is the file empty?", Severity::Warning);
    }
    logger.Log("File \"" + filename + "\" loaded successfully. Loaded total of " + std::to_string(lines.size()) + " lines.", Severity::Debug);
    return true;
}

DataContext TextFileDiffer::generateContext(size_t offset) const {
    const size_t context_before_len = 4;
    const size_t context_after_len = 2;

    DataContext result;

    result.rawData = Utility::vectorSlice(lines, (offset - context_before_len > offset) ? 0 : (offset - context_before_len), offset + context_after_len);

    size_t lastBeforeIndexIncl = std::min((offset < context_after_len) ? offset : context_before_len, offset);
    result.maxIdxOfBeforeContext = lastBeforeIndexIncl;
    result.maxIdxOfOnContext = result.maxIdxOfBeforeContext + 1; // Assume one line changed

    return result;
}

std::string TextFileDiffer::normalizeText(std::string text) const {
    if (!(int)(LoadMode::Text_StrictWhitespace & mode)) {
        // No strict whitespaces, ignore them
        text.erase(remove_if(text.begin(), text.end(), isspace), text.end());
    }

    if (!(int)(LoadMode::Text_StrictCase & mode)) {
        // No strict case, ignore case
        transform(text.begin(), text.end(), text.begin(), tolower);
    }

    return text;
}

