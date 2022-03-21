# Diffeek

**diffeek** - Mime aware diff with custom per-file graphical output

# SYNOPSIS

**diffeek** \[*OPTIONS*\] *SRC_FILE* *DST_FILE*

# DESCRIPTION

**Diffeek** is a command line tool that provides rich mime-aware diff.

# OPTIONS

**Diffeek** tries to guess some options based on file extension, use
options below to override those guesses.

**-h**, **--help**

:   Display help options

**-F format**

:   Force specific fileformat (so far supported: text, binary,
    directory, ppm).

**-s**

:   Strict mode: turn off any format-specific ignores. This implies
    --text-strictcase* and --text-strictwhitespace

**--text-strictcase**, **--text-strictwhitespace**

:   Do not ignore case/whitespace when comparing text files

**-x N**, **--precision N**

:   Set precision to N, where N has to be greater or equal to 2. The bigger precision, the better diffing results, and
    the slower the whole operation is. Set to 0 for maximum precision (guaranteed to find shortest edit distance).
    This is set to roughly (sum of lines of both files)/25 by default.

**-p**, **--patch**

:   Force patch file format output. Should be recognized by the patch(1)
    utility.

**-g**, **--graphical**

:   Force graphical output format. This contains ANSI color codes as
    well. It will look best at terminals that support 8-bit ANSI colors.

Furthermore, Diffeek expects two filenames to compare. First one is source, the second one is destination. They might be directories, in which case folder structure and file changes are compared.

:   

Graphical output, if not forced by user, is determined by Diffeek. If output is determined to be a file, or if we detect piping into another program, patchfile file format will be used by default. Likewise, if output is determined to go on user\'s terminal and is read by human, graphical output will be used by default. The only exception is when diffing graphical file formats (such as PPM), in which case XOR\'ed graphical output will be printed to the output in corresponding file format. In order to override this behaviour, see the -F flag, to compare those files byte-by-byte or line-by-line (binary or textual comparsion).

:   

# BUGS

Report all bugs at https://github.com/SoptikHa2/diffeek.

# AUTHOR

Petr Šťastný \<stastpe8\@fit.cvut.cz\>

# SEE ALSO

diff(1), delta (https://github.com/dandavison/delta)

# LICENCE

**diffeek** is available under the terms of the GNU General Public
License, version 3 or any later version, as published by the Free
Software Foundation.
