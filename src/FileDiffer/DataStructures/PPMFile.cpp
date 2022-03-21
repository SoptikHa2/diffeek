#include "PPMFile.h"

std::optional<PPMFile> PPMFile::Load(const std::string &filename, Logger & logger) {
    PPMFile result(logger);
    std::ifstream stream (filename);

    if (!stream) {
        logger.Log("Failed to open file \"" + filename + "\", make sure the file exists, has some content and is accessible.", Severity::Critical);
        return {};
    }

    std::string header;
    stream >> header;

    if (header == "P1") {
        result.version = 1;
    } else if (header == "P2") {
        result.version = 2;
    } else if (header == "P3") {
        result.version = 3;
    } else {
        logger.Log("Unknown PNM file header: \"" + header + "\", aborting. Note that binary PNM formats are not supported. Use P1, P2, or P3.", Severity::Critical);
        return {};
    }

    stream >> result.width;
    stream >> result.height;
    if (result.version > 1) {
        stream >> result.colorDepth;
    } else {
        result.colorDepth = 1;
    }

    if (!stream) {
        logger.Log("Reading failed after trying to read header: \"" + filename + "\". Make sure the file is fine.", Severity::Critical);
    }
    if (result.width * result.height == 0) {
        logger.Log("Bad PNM file header: \"" + filename + "\". The resulting size would be zero.", Severity::Critical);
        return {};
    }
    if (result.version > 1 && result.colorDepth == 0) {
        logger.Log("Bad PNM file header: \"" + filename + "\". The color depth appears to be zero, which is invalid.", Severity::Critical);
        return {};
    }

    // Read the data itself
    size_t toRead = result.width * result.height * (result.version <= 2 ? 1 : 3);

    for (size_t i = 0; i < toRead; i++) {
        unsigned val;
        stream >> val;
        if (!stream) {
            logger.Log("Failed to read byte " + std::to_string(i) + " from the file \"" + filename +"\". Maybe bad header? Aborting.", Severity::Critical);
            return {};
        }
        if (val > (result.version == 1 ? 1 : result.colorDepth)) {
            logger.Log("Found value bigger than allowed color depth at byte " + std::to_string(i) + " from the file \"" + filename + "\".", Severity::Warning);
        }
        result.data.emplace_back(val);
    }

    return result;
}

std::ostream &operator<<(std::ostream &stream, const PPMFile &rhs) {
    stream << "P" << rhs.version << std::endl;
    stream << rhs.width << " " << rhs.height << std::endl;
    if (rhs.version != 1) stream << rhs.colorDepth << std::endl;

    size_t valuesPerPixel = rhs.version <= 2 ? 1 : 3;
    size_t k = 0;
    for (size_t row = 0; row < rhs.height; row++) {
        for (size_t column = 0; column < rhs.width; column++) {
            for (size_t nthColor = 0; nthColor < valuesPerPixel; nthColor++) {
                stream << rhs.data[k++] << " ";
            }
            if (valuesPerPixel > 1) stream << "  ";
        }
        stream << std::endl;
    }
    return stream;
}

void PPMFile::enlarge(size_t targetWidth, size_t targetHeight) {
    size_t extraWidth = (targetWidth > width ? targetWidth - width : 0);
    size_t extraHeight = (targetHeight > height ? targetHeight - height : 0);
    std::vector<unsigned> newData;

    // Add more columns
    size_t extraValues = extraWidth * (version <= 2 ? 1 : 3);
    size_t currentValues = width * (version <= 2 ? 1 : 3);
    // For all rows...
    size_t k = 0;
    for (size_t row = 0; row < height; row++) {
        // Skip [currentValues] values
        for (size_t l = 0; l < currentValues; l++) newData.push_back(k++ < data.size() ? data[k-1] : 0);
        // Add [extraValues] values
        for (size_t l = 0; l < extraValues; l++) newData.push_back(0);
    }

    // Add more rows
    size_t pixelsPerRow = (width + extraWidth)  * (version <= 2 ? 1 : 3);
    for (size_t i = 0; i < extraHeight * pixelsPerRow; i++) {
        newData.push_back(0);
    }

    data = std::move(newData);
    width += extraWidth;
    height += extraHeight;
}

PPMFile & PPMFile::XORWith(const PPMFile &other) {
    PPMFile rhs = other;

    // First of all, make sure both images have equal version, otherwise it's useless to xor them
    this->upgradeVersion(rhs.version, rhs.colorDepth);
    rhs.upgradeVersion(this->version, this->colorDepth);
    // Next, make sure that both files have equal size.
    // Enlarge them if necessary.
    this->enlarge(other.width, other.height);
    rhs.enlarge(this->width, this->height);
    assert(this->width == rhs.width);
    assert(this->height == rhs.height);
    assert(this->data.size() == rhs.data.size());

    // Now, generate diff file:
    for(size_t i = 0; i < this->data.size(); i++) {
        this->data[i] ^= rhs.data[i];
    }

    return *this;
}

void PPMFile::upgradeVersion(unsigned int targetVersion, size_t targetColorDepth) {
    if (targetVersion <= version) return;

    // Upgrade to P2
    if (version == 1) {
        // Make all black pixels have their maximum gray value.
        std::transform(data.begin(), data.end(), data.begin(), [targetColorDepth](unsigned value) -> unsigned {
            if (value == 0) return 0;
            return targetColorDepth;
        });
        version++;
    }

    // Upgrade to P3
    if (version == 2 && targetVersion == 3) {
        std::vector<unsigned> newData;
        for(unsigned value : data) {
            for (int i = 0; i < 3; ++i) {
                newData.push_back((unsigned)((float)targetColorDepth * ((float)value/(float)colorDepth)));
            }
        }
        version++;
    }

    colorDepth = targetColorDepth;
}

