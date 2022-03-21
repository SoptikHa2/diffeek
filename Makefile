CXX=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wno-long-long -O2 -g
LDFLAGS=-lstdc++fs
CXXFLAGS_DEBUG=-std=c++17 -Werror -Wall -pedantic -Wno-long-long -g -pg -fPIE -fsanitize=address
TARGET=stastpe8

# find src/ -iname '*.cpp' | grep -v 'cmake'
SOURCES=src/Utility.cpp src/FileDiffer/TextFileDiffer.cpp src/FileDiffer/PPMFileDiffer.cpp src/FileDiffer/DataStructures/PPMFile.cpp src/FileDiffer/DataStructures/LevenshteinMatrix.cpp src/FileDiffer/DirectoryDiffer.cpp src/FileDiffer/BinaryFileDiffer.cpp src/FileDiffer/FileDiffer.cpp src/Logger/StderrLogger.cpp src/Logger/Logger.cpp src/Diffeek.cpp src/ArgParser.cpp src/main.cpp src/DataOutput/DirectOutput.cpp src/DataOutput/DataDifference.cpp src/DataOutput/DataOutput.cpp src/DataOutput/GraphicalOutput.cpp src/DataOutput/PatchfileOutput.cpp

# find src/ -iname '*.cpp' | grep -v 'cmake' | sed 's/.cpp/.o/' | sed 's/.*\/(.*)$/\1/' -E | sed 's/^/build\//'
OBJECTS=build/Utility.o build/TextFileDiffer.o build/PPMFileDiffer.o build/PPMFile.o build/LevenshteinMatrix.o build/DirectoryDiffer.o build/BinaryFileDiffer.o build/FileDiffer.o build/StderrLogger.o build/Logger.o build/Diffeek.o build/ArgParser.o build/main.o build/DirectOutput.o build/DataDifference.o build/DataOutput.o build/GraphicalOutput.o build/PatchfileOutput.o

.PHONY: all diffeek run clean doc test compile install uninstall builddir
.DEFAULT_GOAL=all

all: diffeek doc test

compile: diffeek

builddir:
	mkdir -p build

diffeek: builddir $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

run: diffeek
	./$(TARGET)

clean:
	rm -rf $(TARGET) build doc

doc: Doxyfile
	doxygen Doxyfile

test: diffeek
	examples/automated-check.sh ./$(TARGET) tests

# > V Makefile smí být použity pouze tyto programy: rm, gcc, g++, mkdir, doxygen, cp, mv, cd, ar, make.
#install: diffeek
#	install -s $(TARGET) -t $$HOME/.local/bin
#	install diffeek.1 -m=644 -t "$$(manpath | cut -d':' -f1)/man1"
#	mandb
#
#uninstall:
#	rm $$HOME/.local/bin/$(TARGET)
#	rm "$$(manpath | cut -d':' -f1)/man1/diffeek.1"

%.o:
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# find src/ -iname '*.cpp' | grep -v 'cmake' | xargs g++ -MM | sed '/^[^ ]/ { s/^/build\// }' 
build/Utility.o: src/Utility.cpp
build/TextFileDiffer.o: src/FileDiffer/TextFileDiffer.cpp \
 src/FileDiffer/TextFileDiffer.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h
build/PPMFileDiffer.o: src/FileDiffer/PPMFileDiffer.cpp \
 src/FileDiffer/PPMFileDiffer.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h \
 src/FileDiffer/DataStructures/PPMFile.h
build/PPMFile.o: src/FileDiffer/DataStructures/PPMFile.cpp \
 src/FileDiffer/DataStructures/PPMFile.h \
 src/FileDiffer/DataStructures/../../Logger/Logger.h
build/LevenshteinMatrix.o: src/FileDiffer/DataStructures/LevenshteinMatrix.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h
build/DirectoryDiffer.o: src/FileDiffer/DirectoryDiffer.cpp \
 src/FileDiffer/DirectoryDiffer.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h
build/BinaryFileDiffer.o: src/FileDiffer/BinaryFileDiffer.cpp \
 src/FileDiffer/BinaryFileDiffer.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h
build/FileDiffer.o: src/FileDiffer/FileDiffer.cpp src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h \
 src/FileDiffer/BinaryFileDiffer.h src/FileDiffer/TextFileDiffer.h \
 src/FileDiffer/PPMFileDiffer.h src/FileDiffer/DataStructures/PPMFile.h \
 src/FileDiffer/DirectoryDiffer.h
build/StderrLogger.o: src/Logger/StderrLogger.cpp src/Logger/StderrLogger.h \
 src/Logger/Logger.h
build/Logger.o: src/Logger/Logger.cpp src/Logger/Logger.h
build/Diffeek.o: src/Diffeek.cpp src/Diffeek.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h src/ArgParser.h
build/ArgParser.o: src/ArgParser.cpp src/ArgParser.h \
 src/FileDiffer/FileDiffer.h src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h
build/main.o: src/main.cpp src/Diffeek.h src/FileDiffer/FileDiffer.h \
 src/FileDiffer/../Logger/Logger.h \
 src/FileDiffer/../DataOutput/DataDifference.h \
 src/FileDiffer/../DataOutput/../Utility.cpp \
 src/FileDiffer/../Utility.cpp \
 src/FileDiffer/DataStructures/LevenshteinMatrix.h src/ArgParser.h \
 src/DataOutput/DataOutput.h src/DataOutput/../Utility.cpp \
 src/Logger/StderrLogger.h
build/DirectOutput.o: src/DataOutput/DirectOutput.cpp \
 src/DataOutput/DirectOutput.h src/DataOutput/DataOutput.h \
 src/DataOutput/DataDifference.h src/DataOutput/../Utility.cpp \
 src/DataOutput/../ArgParser.h src/DataOutput/../FileDiffer/FileDiffer.h \
 src/DataOutput/../FileDiffer/../Logger/Logger.h \
 src/DataOutput/../FileDiffer/../Utility.cpp \
 src/DataOutput/../FileDiffer/DataStructures/LevenshteinMatrix.h
build/DataDifference.o: src/DataOutput/DataDifference.cpp \
 src/DataOutput/DataDifference.h src/DataOutput/../Utility.cpp
build/DataOutput.o: src/DataOutput/DataOutput.cpp src/DataOutput/DataOutput.h \
 src/DataOutput/DataDifference.h src/DataOutput/../Utility.cpp \
 src/DataOutput/../ArgParser.h src/DataOutput/../FileDiffer/FileDiffer.h \
 src/DataOutput/../FileDiffer/../Logger/Logger.h \
 src/DataOutput/../FileDiffer/../Utility.cpp \
 src/DataOutput/../FileDiffer/DataStructures/LevenshteinMatrix.h \
 src/DataOutput/PatchfileOutput.h src/DataOutput/GraphicalOutput.h \
 src/DataOutput/DirectOutput.h
build/GraphicalOutput.o: src/DataOutput/GraphicalOutput.cpp \
 src/DataOutput/GraphicalOutput.h src/DataOutput/DataOutput.h \
 src/DataOutput/DataDifference.h src/DataOutput/../Utility.cpp \
 src/DataOutput/../ArgParser.h src/DataOutput/../FileDiffer/FileDiffer.h \
 src/DataOutput/../FileDiffer/../Logger/Logger.h \
 src/DataOutput/../FileDiffer/../Utility.cpp \
 src/DataOutput/../FileDiffer/DataStructures/LevenshteinMatrix.h
build/PatchfileOutput.o: src/DataOutput/PatchfileOutput.cpp \
 src/DataOutput/PatchfileOutput.h src/DataOutput/DataOutput.h \
 src/DataOutput/DataDifference.h src/DataOutput/../Utility.cpp \
 src/DataOutput/../ArgParser.h src/DataOutput/../FileDiffer/FileDiffer.h \
 src/DataOutput/../FileDiffer/../Logger/Logger.h \
 src/DataOutput/../FileDiffer/../Utility.cpp \
 src/DataOutput/../FileDiffer/DataStructures/LevenshteinMatrix.h
