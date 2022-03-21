#include "PatchfileOutput.h"


/// Write patchfile format to specified output.
///
/// \param logger
/// \param outputStream
///
/// /**************************/
///   PATCHFILE FILE FORMAT
/// /**************************/
///
/// Patchfile may optionally begin with lines
/// --- a/foo.c
/// +++ b/foo.c
/// , designating the names of files changed.
///
/// Afterwards, the file consists of one or more blocks.
/// Each block starts with a @@ header.
///
/// The header looks like this:
/// @@ -R,r +R,r @@
/// where `R` specifies the line number where
/// the current block starts in source and destination file,
/// and how many lines does the block last in each file.
///
/// Afterwards, there is a number of lines, that are either
/// context lines (prefixed with a single space) that are
/// the same in both files, `-` lines that are only in the
/// source file, and `+` lines, that are only in the
/// destination file.
///
/// Source: https://www.oreilly.com/library/view/git-pocket-guide/9781449327507/ch11.html
PatchfileOutput::PatchfileOutput(Logger &logger) : DataOutput(logger) { }

bool PatchfileOutput::Write(const DataDifference &difference) {
    // Write file header
    std::cout << "--- " << difference.sourceName << std::endl;
    std::cout << "+++ " << difference.destinationName << std::endl;

    for(const auto & delta : difference.deltas) {
        // Write delta header
        std::cout << "@@ -" << delta.indicatorSource << "," << delta.indicatorSourceLen << " +"
                      << delta.indicatorDestination << "," << delta.indicatorDestLen << " @@" << std::endl;
        switch(delta.kind) {
            case Addition:
                for (const auto & deltaInfo : delta.deltaInfo) {
                    std::cout << "+" << deltaInfo << std::endl;
                }
                break;
            case Deletion:
                for (const auto & deltaInfo : delta.deltaInfo) {
                    std::cout << "-" << deltaInfo << std::endl;
                }
                break;
            case Modification: {
                size_t k = delta.sourceFileContext.maxIdxOfBeforeContext;
                for (const auto & deltaInfo : delta.deltaInfo) {
                    if (k < delta.sourceFileContext.rawData.size())
                        std::cout << "-" << delta.sourceFileContext.rawData[k] << std::endl;
                    std::cout << "+" << deltaInfo << std::endl;
                    k++;
                }
                break;
            }
            default:
                throw std::logic_error("Missing delta case: patchfile output");
        }
    }

    return std::cout.good();
}
