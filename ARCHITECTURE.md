# Architecture

First of all, in `main.cpp`, arguments are parsed using `ArgParser`, and logger is set up.

Afterwards, `Diffeek` is constructed and called, which provides the core of the applications. It loads and diffs two files.

The difference is then passed into one of the `DataOutput` classes (GraphicalOutput for user, PatchfileOutput for computer).

Diffeek itself construct one of the `FileDiffer` classes per each file. The classes load data from file into target format, and, if applicable, diffs it. 

Difference data is stored in `DataDifference`. It is returned from Diffeek, and is expected from the user to handle and process, possibly piping it into one of the `DataOutput` classes.

