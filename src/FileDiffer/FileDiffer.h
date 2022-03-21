#pragma once

#include "../Logger/Logger.h"
#include "../DataOutput/DataDifference.h"
#include "../Utility.cpp"
#include "DataStructures/LevenshteinMatrix.h"

#include <fstream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <memory>
#include <optional>
#include <fstream>

/// File formats that are supported, and may be forced by user.
enum DataFormat {
    Automatic,
    Binary,
    Text,
    PPM,
    Directory,
};

/// Load modes that signify permissiveness of the comparison algorithms used.
/// This may be used to eg ignore case or whitespace.
enum LoadMode {
    Permissive = 1 << 0,
    Strict = 1 << 1,
    Text_StrictCase = 1 << 2,
    Text_StrictWhitespace = 1 << 3
};

/// Or LoadMode.
///
/// \param lhs First LoadMode
/// \param rhs Second LoadMode
/// \return OR-ed enum, so all values that are in first or second parameter
LoadMode operator|(const LoadMode &lhs, const LoadMode &rhs);

/// Add enum variants from second enum to the first one.
///
/// \param lhs First LoadMode
/// \param rhs Second LoadMode
/// \return Reference to first enum, which was changed by the OR.
LoadMode & operator|=(LoadMode &lhs, const LoadMode &rhs);

/// And LoadMode, so create new enum with values, that are in both operands.
///
/// \param lhs First LoadMode
/// \param rhs Second LoadMode
/// \return New enum, that contains variants that are in both the first and second enum.
LoadMode operator&(const LoadMode & lhs, const LoadMode &rhs);

/// Generic class that is able to load a file, and later produce diff between itself and another
/// instance of it's kind.
class FileDiffer {
protected:
    const LoadMode &mode;
    Logger &logger;
    std::optional<size_t> precision;
    std::ifstream inputStream;
    const std::string filename;

public:
    FileDiffer(const LoadMode &mode, Logger &logger, std::optional<size_t> precision, std::string filename) :
        mode(mode), logger(logger), precision(precision), filename(std::move(filename)) {}

    virtual ~FileDiffer() = default;

    /// Depending on target file and arguments specified by user, construct and get instance of one of the subclasses.
    ///
    /// \param filename Target filename
    /// \param mode Specific load mode (permissiveness) forced by user, if any
    /// \param logger logger
    /// \return Instance of one of the subclasses
    static std::unique_ptr<FileDiffer> GetInstance(const std::string &filename, const LoadMode &mode, std::optional<size_t> precision, Logger &logger);

    /// Get instance of one diffing subclass, as forced by user.
    ///
    /// \param forcedFormat File format that is expected to be in the file. No questions asked. Target subclass will be returned.
    /// \param filename Target filename
    /// \param mode Specific load mode (permissiveness) forced by user, if any
    /// \param logger logger
    /// \return Instance of one of the subclasses forced
    static std::unique_ptr<FileDiffer>
    GetInstance(DataFormat forcedFormat, const std::string &filename, const LoadMode &mode, std::optional<size_t> precision, Logger &logger);

    /// Try to load data into memory from the given file. This is a prereq for calling Diff.
    /// Loading progress may be logged into logger, depending on severity.
    ///
    /// \return Whether loading succeeded.
    virtual bool LoadData() = 0;

    /// Create difference between this and other instance. The other instance should be of the same or compatible type,
    /// otherwise, the result is unspecified. This shall be called after successfully calling `LoadData`, otherwise
    /// the result is unspecified.
    ///
    /// \param other Other instance. This has to be the same or compatible type. An error will be reported otherwise.
    /// \return Nothing if an error occurred, or data difference between two files loaded.
    virtual std::optional<DataDifference> Diff(const FileDiffer &other) const = 0;

protected:
    /// Generate context of source file at given offset.
    ///
    /// Context generally contains some of the lines before and
    /// after given context, so user can see, where did the change
    /// actually happen. The context is also sometimes
    /// used to display the diff itself - for example when modifying,
    /// it is used to display the old value.
    /// \param offset Offset from start of file. Depending on data type, this may be lines, bytes, etc.
    /// \return DataContext structure around the given offset.
    virtual DataContext generateContext(size_t offset) const = 0;

    /// Construct levenshtein matrix from two vectors of type T.
    ///
    /// The type has to provide `==` operator. The caller is expected to delete [] result themselves.
    ///
    /// See more here: https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_full_matrix
    ///
    /// \tparam T The chunk type - typically string (for line) or char (for bytes). Can be even hashes or anything else.
    /// \param source Vector of chunks of source file.
    /// \param destination Vector of chunks of source file.
    /// \return Levenshtein matrix of these two vectors, where each chunk is processed as if it was standalone
    /// letter. Two chunks that compare the same are considered equal, and are represented intuitively
    /// in the matrix, as one would except.
    //
    // This has to be implemented in header, bc of templates.
    // The implementation of a non-specialized template must be visible to a translation unit that uses it.
    template<typename T>
    LevenshteinMatrix constructLevenshteinMatrix(const std::vector<T> &source, const std::vector<T> &destination) const {
        LevenshteinMatrix matrix(destination.size()+1, source.size()+1, precision);

        // Fill source and target prefix
        for (size_t i = 0; i <= source.size(); ++i) {
            matrix.Set(i, 0, i);
        }
        for (size_t j = 0; j <= destination.size(); ++j) {
            matrix.Set(0, j, j);
        }

        auto increment = [](size_t k) -> size_t { return k + 1; };
        // Fill rest of the matrix
        for (size_t i = 1; i <= source.size(); ++i) {
            for (size_t j = 1; j <= destination.size(); ++j) {
                size_t modificationCost = (source[i-1] == destination[j-1]) ? 0 : 1;

                matrix.Set(i, j, *Utility::min<std::optional<size_t>>(
                        Utility::apply<size_t>(matrix.Get(i - 1, j), increment), // Deletion
                        Utility::apply<size_t>(matrix.Get(i, j - 1), increment), // Addition
                        Utility::apply<size_t>(matrix.Get(i - 1, j - 1),
                           [modificationCost](size_t k) { return k + modificationCost; }), // Modification
                        Utility::cmpOptionals<size_t>)
                );
            }
        }

        return matrix;
    }

    /// Find cheapest path from bottom-right corner of the levenshtein matrix to the top-left corner.
    /// This also generates the necessary edit sequence, needed to edit the file.
    ///
    /// \tparam T The chunk type - typically string (for line) or char (for bytes). Can be even hashes or anything else.
    /// \param matrix Levenshtein matrix of the source and destination
    /// \param source Vector of chunks of source file.
    /// \param destination Vector of chunks of source file.
    /// \param dstFilename Destination filename, to be written in difference log.
    /// \param sourceDataDisplayFunction Function that, given size_t index, returns user-presentable information
    /// about the chunk. Might not be the same, as the `source` chunk. (Eg `source` might contain bytes as `vector<char>`,
    /// while this function might return their textual representation as std::string (eg 0x25 and such)).
    /// \param destinationDataDisplayFunction Function that, given size_t index, returns user-presentable information
    /// about the chunk. Might not be the same, as the `source` chunk. (Eg `source` might contain bytes as `vector<char>`,
    /// while this function might return their textual representation as std::string (eg 0x25 and such)).
    /// \return Data difference described by the matrix, with recorded information about context and filenames.
    //
    // This has to be implemented in header, bc of templates.
    // The implementation of a non-specialized template must be visible to a translation unit that uses it.
    template<typename T>
    DataDifference
    levenshteinMatrixToDeltas(const LevenshteinMatrix & matrix, const std::vector<T> &source, const std::vector<T> &destination,
                              const std::string &dstFilename,
                              const std::function<std::string(size_t idx)>& sourceDataDisplayFunction,
                              const std::function<std::string(size_t idx)>& destinationDataDisplayFunction) const {
        DataDifference result (filename, dstFilename, {}, ForcedOutput::Default);

        size_t i = source.size(), j = destination.size();

        while (i != 0 || j != 0) {
            // Get neighbours: top, left, topleft. Choose smallest one.
            // This will be part of the lowest edit distance.
            std::optional<size_t> leftVal = {}, topVal = {}, leftTopVal = {};

            if (i != 0) topVal = matrix.Get(i - 1, j);
            if (j != 0) leftVal = matrix.Get(i, j - 1);
            if (i != 0 && j != 0) leftTopVal = matrix.Get(i - 1, j - 1);

            // I know that at least one of the optionals will have a value. Take minimum from those.
            auto minimum = Utility::min<std::optional<size_t>>(leftVal, topVal, leftTopVal, Utility::cmpOptionals<size_t>);

            if (minimum == leftTopVal) /* either no change or modification */ {
                if (matrix.Get(i, j) ==
                    matrix.Get(i - 1, j - 1)) {
                    // No change, just follow the path for free
                    i--;
                    j--;
                } else {
                    // A change happened
                    result.deltas.push_back(DataDelta(
                                                    DataDeltaKind::Modification,
                                                    {destinationDataDisplayFunction(j-1)},
                                                    generateContext(i-1),
                                                    i, j, 1, 1
                        ));
                    i--;
                    j--;
                }
            } else if (minimum == leftVal) /* addition */ {
                result.deltas.push_back(DataDelta(
                                                DataDeltaKind::Addition,
                                                {destinationDataDisplayFunction(j-1)},
                                                generateContext(i-1),
                                                i, j, 0, 1
                                        ));
                j--;
            } else if (minimum == topVal) /* deletion */ {
                result.deltas.push_back(DataDelta(
                                                DataDeltaKind::Deletion,
                                                {sourceDataDisplayFunction(i-1)},
                                                generateContext(i-1),
                                                i, j, 1, 0
                                        ));
                i--;
            } else {
                throw std::logic_error(
                        "Error while diffing: found a change in diff matrix that doesn't correspond to anything.");
            }
        }

        result.deltas.reverse();
        return result;
    }
};
