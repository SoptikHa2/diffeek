#pragma once

#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>

#include "../../Logger/Logger.h"

/// PPM file container. Contains image mapped to memory.
class PPMFile {
    /// PPM file version. 1 for P1, 2 for P2, and 3 for P3.
    unsigned version;
    ///  Width and height of the stored image file.
    size_t width, height;
    /// Color depth. Is exactly `1` for P1, `n` for P2, where `n` is count of
    /// various shades of gray, or `k` for P3, where `k` is how many values are there
    /// in red (or blue, or green) components of color.
    size_t colorDepth;
    Logger & logger;
    /// Image data, contains numbers - representing grayscale/colorful image, depending on
    /// version. In case of P3, it takes three values to encode one pixel.
    std::vector<unsigned> data;

private:
    PPMFile(Logger & logger) : logger(logger) {}

public:
    /// XOR one image with another one. This is done by XORing individual values, or pixels.
    /// The current image will be modified by XORing, and might change both dimensions
    /// and picture version, to match the right operand.
    ///
    /// \param rhs Other image to xor with
    /// \return Returns reference to this, so XOR operation is chainable.
    PPMFile & XORWith(const PPMFile & rhs);

    /// Try to load PPM file from filename to memory. If this fails,
    /// details will be logged.
    ///
    /// \param filename Filename of the PPM file. Only P1, P2, P3 are supported.
    /// \param logger Logger
    /// \return If successful, PPMFile instance. Otherwise, nothing.
    static std::optional<PPMFile> Load(const std::string & filename, Logger & logger);

    /// Print PPM image file to stream. This produces valid P1/P2/P3 file.
    ///
    /// \param stream Output stream
    /// \param rhs Image to write to stream
    /// \return Reference to output stream
    friend std::ostream & operator<<(std::ostream & stream, const PPMFile & rhs);

private:
    /// Enlarge the image up to [targetWidth] and [targetHeight]. If
    /// one (or both) of the dimensions is smaller than the image
    /// already is, ignore it and don't do anything in the dimension.
    /// New pixels will be black (value 0).
    ///
    /// \param targetWidth Target width in pixels
    /// \param targetHeight Target height in pixels
    void enlarge(size_t targetWidth, size_t targetHeight);
    /// Upgrade PPM version to higher one.
    ///
    /// \param targetVersion Target version to upgrade to. If it
    /// is lower or equal to current one, nothing will change.
    void upgradeVersion(unsigned targetVersion, size_t targetColorDepth);
};
