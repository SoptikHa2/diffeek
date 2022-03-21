cmake_minimum_required(VERSION 3.17)
project(src)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(DataOutput)
include_directories(DataStream)
include_directories(FileDiffer)

add_executable(src
        DataOutput/DataOutput.cpp
        DataOutput/DataOutput.h
        DataOutput/GraphicalOutput.cpp
        DataOutput/GraphicalOutput.h
        DataOutput/PatchfileOutput.cpp
        DataOutput/PatchfileOutput.h
        FileDiffer/BinaryFileDiffer.cpp
        FileDiffer/BinaryFileDiffer.h
        FileDiffer/DirectoryDiffer.cpp
        FileDiffer/DirectoryDiffer.h
        FileDiffer/FileDiffer.h
        FileDiffer/FileDiffer.cpp
        FileDiffer/PPMFileDiffer.cpp
        FileDiffer/PPMFileDiffer.h
        FileDiffer/TextFileDiffer.cpp
        FileDiffer/TextFileDiffer.h
        ArgParser.cpp
        ArgParser.h
        Diffeek.cpp
        Diffeek.h
        main.cpp Logger/Logger.h Logger/StderrLogger.cpp Logger/StderrLogger.h Logger/Logger.cpp DataOutput/DataDifference.cpp DataOutput/DataDifference.h Utility.cpp DataOutput/DirectOutput.cpp DataOutput/DirectOutput.h FileDiffer/DataStructures/PPMFile.cpp FileDiffer/DataStructures/PPMFile.h FileDiffer/DataStructures/LevenshteinMatrix.cpp FileDiffer/DataStructures/LevenshteinMatrix.h)
