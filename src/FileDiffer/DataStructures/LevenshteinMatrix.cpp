#include "LevenshteinMatrix.h"

LevenshteinMatrix::LevenshteinMatrix(size_t rows, size_t cols, std::optional<size_t> precision) : rows(rows), cols(cols) {
    if (precision.has_value()) this->precision = *precision;
    else this->precision = (rows + cols) / 25;
}

std::optional<size_t> LevenshteinMatrix::Get(size_t row, size_t col) const {
    if (isOutsidePrecisionRange(row, col)) return {};
    if (values.find(getCoord(row, col)) == values.end()) return {};
    return values.at(getCoord(row, col));
}

LevenshteinMatrix & LevenshteinMatrix::Set(size_t row, size_t col, size_t value) {
    if (!isOutsidePrecisionRange(row, col))
        values[getCoord(row, col)] = value;
    return *this;
}

size_t LevenshteinMatrix::getCoord(size_t row, size_t col) const {
    return cols * col + row;
}

bool LevenshteinMatrix::isOutsidePrecisionRange(size_t row, size_t col) const {
    if (precision == 0) return false;

    if (row == 0 || col == 0) return false;
    if (row == cols-1 || col == rows-1) return false;

    size_t rowOfDiagonalItemAtGivenCol = (size_t)(((double)col / (double)rows) * (double)cols);
    size_t colOfDiagonalItemAtGivenRow = (size_t)(((double)row / (double)cols) * (double)rows);

    if (row > precision && row - precision > rowOfDiagonalItemAtGivenCol)
        return true;

    if (col > precision && col - precision > colOfDiagonalItemAtGivenRow)
        return true;

    return false;
}

std::ostream & operator<<(std::ostream & stream, const LevenshteinMatrix & matrix) {
    stream << "Levenshtein matrix " << matrix.rows << "x" << matrix.cols <<": " << std::endl;
    stream << "Values matrix:" << std::endl;
    for(size_t i = 0; i < matrix.cols; i++) {
        for(size_t j = 0; j < matrix.rows; j++) {
            stream << std::setw(2);
            stream << (!matrix.Get(i, j).has_value() ? "??" : std::to_string(*matrix.Get(i, j))) << " ";
        }
        stream << std::endl;
    }
    stream << "Validity matrix:" << std::endl;
    for(size_t i = 0; i < matrix.cols; i++) {
        for(size_t j = 0; j < matrix.rows; j++) {
            stream << matrix.isOutsidePrecisionRange(i, j);
        }
        stream << std::endl;
    }
    return stream;
}
