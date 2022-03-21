#pragma once

#include <unordered_map>
#include <cstddef>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <optional>

/// Represents levenshtein matrix, which describes edit distance
/// between two data sets.
///
/// This has capability to set *precision* - part of the matrix
/// will be not computed not stored in memory, saving
/// some of the processing power in exchange for
/// precision and good edit distance.
///
/// The result will still be valid even with very small
/// precision, just less efficient. In other words,
/// the resulting patch(file) will still be correct,
/// but may be bigger.
class LevenshteinMatrix {
    /// Levenshtein matrix values, mapping key: see [getCoords]
    std::unordered_map<size_t, size_t> values;
    /// Rows, cols and set precision of the matrix
    size_t rows, cols, precision;

public:
    /// Construct new levenshtein matrix
    ///
    /// \param rows Rows of the matrix (should be length of word + 1)
    /// \param cols Columns of the matrix (should be length of word + 1)
    /// \param precision Precision of the matrix. If not set, it will be determined
    /// automatically, depending on size of input. Only coords, which are at most
    /// [precision] tiles away from main diagonalle will be computed and saved.
    /// Set preicisiont o 0 for unlimited precision. Minimum recommended
    /// precision is at the very least 2. Otherwise, buggy behaviour may
    /// occur.
    LevenshteinMatrix(size_t rows, size_t cols, std::optional<size_t> precision);

    /// Return value stored if matrix, if there is any on given coords already
    /// and is within precision range.
    std::optional<size_t> Get(size_t row, size_t col) const;
    /// Set value of the matrix at given coordinates to a value.
    /// This is NOP if the coords are outside precision range.
    LevenshteinMatrix & Set(size_t row, size_t col, size_t value);

private:
    /// Return 1D coordinates, mapped from two of them.
    /// This allows us to store values in one-dimensional map.
    size_t getCoord(size_t row, size_t col) const;
    /// Check, if given coords are at least [precision] tiles
    /// away from the main diagonalle, or at the edges of the matrix.
    /// If the given coord is not at one of these positions, it is determined
    /// to be out of range.
    ///
    /// \param row Row coordinate
    /// \param col Column coordinate
    /// \return Whether the coordinate is outside set preicison range
    bool isOutsidePrecisionRange(size_t row, size_t col) const;

    friend std::ostream & operator<<(std::ostream & stream, const LevenshteinMatrix & matrix);
};

std::ostream & operator<<(std::ostream & stream, const LevenshteinMatrix & matrix);
