#include "Grep.h"
//#include "Grep.cpp"
#include "Helper_functions.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string pattern;
    std::string filename;
    std::string patternFile;
    bool caseInsensitive = false;
    bool invertMatch = false;
    bool showLineNumbers = false;
    bool countOnly = false;
    bool hideFilename = false;
    bool extendedRegex = false;
    bool usePatternFile = false;

    if (!parseArguments(argc, argv, pattern, filename, patternFile,
                       caseInsensitive, invertMatch, showLineNumbers,
                       countOnly, hideFilename, extendedRegex, usePatternFile)) {
        printUsage();
        return 1;
    }

    if (filename.empty()) {
        std::cerr << "Missing filename\n";
        printUsage();
        return 1;
    }

    if (!usePatternFile && pattern.empty()) {
        std::cerr << "Missing pattern\n";
        printUsage();
        return 1;
    }

    Grep grep(filename, pattern);
    grep.setCaseInsensitive(caseInsensitive);
    grep.setInvertMatch(invertMatch);
    grep.setShowLineNumbers(showLineNumbers);
    grep.setCountOnly(countOnly);
    grep.setHideFilename(hideFilename);
    grep.setExtendedRegex(extendedRegex);

    processPatterns(grep, usePatternFile, patternFile, countOnly);

    return 0;
}