#include <iostream>
#include <optional>
#include "Diffeek.h"
#include "ArgParser.h"
#include "DataOutput/DataOutput.h"
#include "Logger/StderrLogger.h"

void help();

int main(int argc, char ** argv) {
    StderrLogger logger(Severity::Warning);

    std::optional<ArgData> arguments = ArgParser(argc, argv).parse(logger);
    if(!arguments) {
        help();
        return 1;
    }
    if(arguments->showUsage) {
        help();
        return 0;
    }

    if (arguments->verbosity > 0) logger.ChangeSeverity((Severity::Info));
    if (arguments->verbosity > 1) logger.ChangeSeverity((Severity::Debug));

    try {
        Diffeek app(*arguments, logger);
        if(!app.LoadData(logger)) return 2;
        std::optional<DataDifference> data = app.Diff();
        if(!data) /* something failed */ return 3;
        if(!DataOutput::GetOutput(*arguments, *data, logger)->Write(*data)) return 4;
    } catch (const std::exception & e) {
        std::cerr << "A fatal uncaught error occurred while diffing. Aborting." << std::endl;
        std::cerr << e.what() << std::endl;
        return 5;
    }

    return 0;
}

void help() {
    using namespace std;
    cout << "Diffeek (Petr Šťastný)" << endl;
    cout << endl;
    cout << "Refer to manpage diffeek(1) for detailed usage info." << endl;
    cout << endl;
    cout << "Synopsis: diffeek [options] filenameFrom filenameTo" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "\t-h\t\tDisplay this" << endl;
    cout << "\t-F format\t\tforce specific fileformat (text, binary, directory, ppm, ...). Refer to manpage for exhaustive list." << endl;
    cout << "\t-s\t\tstrict mode: don't ignore atribute order in structured files. Also implies --text-strictcase and --text-strictwhitespace." << endl;
    cout << "\t--text-strictcase,\n\t--text-strictwhitespace\t\tdo not ignore case / whitespaces when comparing text files." << endl;
    cout << "\t-x N\t\t--precision N\t\tset diffing precision to N, where N >= 2 (default: 1/25 of sum of lines of a file). This affects speed/minimal found edit distance. Set to 0 for unlimited precision." << endl;
    cout << "\t-p\t\tpatchfile output: force output in form of patchfile (this is the default when outputing to a file)" << endl;
    cout << "\t-g\t\tgraphical output: force output in graphical, user-friendly mode (this is the default when outputing to tty)" << endl;
    cout << endl;
    cout << endl;
    cout << "Diffeek expects two filenames as well. They might be directories, in which case file structure and file changes";
    cout << " are compared." << endl;
    cout << endl;
    cout << "Graphical  output,  if  not forced by user, is determined by Diffeek. If output is determined to be a file, or if we detect piping into another program," << std::endl;
    cout << "   patchfile file format will be used by default. Likewise, if output is determined to go on user's terminal and is read by human, graphical output" << std::endl;
    cout << "   will be used by default. The only exception is when diffing graphical file formats (such as PPM), in which case XOR'ed graphical output" << std::endl;
    cout << "   will be printed to the output in corresponding file format." << std::endl;
    cout << "In order to override this behaviour, see the -F flag, to compare those files byte-by-byte or line-by-line (binary or textual comparsion)." << std::endl;
    cout << endl;
    cout << endl;
    cout << "Author: Petr Šťastný <stastpe8@fit.cvut.cz>" << endl;
}
