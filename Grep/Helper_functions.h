#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

#include <string>
#include <iostream>

inline void printUsage() {
    std::cout << "Usage: grep [options] pattern filename\n"
        << "Options:\n"
        << "  -i    Case-insensitive search\n"
        << "  -v    Invert match (show non-matching lines)\n"
        << "  -n    Show line numbers\n"
        << "  -c    Show only count of matching lines\n"
        << "  -h    Hide filenames in output\n"
        << "  -E    Treat pattern as extended regular expression\n"
        << "  -f    Take patterns from file, one per line\n";
}

inline bool parseArguments(int argc, char* argv[], std::string& pattern, std::string& filename,
    std::string& patternFile, bool& caseInsensitive, bool& invertMatch,
    bool& showLineNumbers, bool& countOnly, bool& hideFilename,
    bool& extendedRegex, bool& usePatternFile) {
    int i = 1;
    while (i < argc) {
        std::string opt = argv[i];
        if (opt[0] == '-') {
            if (opt == "-i") caseInsensitive = true;
            else if (opt == "-v") invertMatch = true;
            else if (opt == "-n") showLineNumbers = true;
            else if (opt == "-c") countOnly = true;
            else if (opt == "-h") hideFilename = true;
            else if (opt == "-E") extendedRegex = true;
            else if (opt == "-f") {
                if (i + 1 >= argc) {
                    std::cerr << "Error: -f option requires a pattern file\n";
                    return false;
                }
                patternFile = argv[++i];
                usePatternFile = true;
            }
            else {
                std::cerr << "Unknown option: " << opt << std::endl;
                return false;
            }
        }
        else {
            if (!usePatternFile && pattern.empty()) {
                pattern = opt;
            }
            else if (filename.empty()) {
                filename = opt;
            }
        }
        i++;
    }
    return true;
}

inline void processPatterns(Grep& grep, bool usePatternFile, const std::string& patternFile, bool countOnly) {
    if (countOnly) {
        if (usePatternFile) {
            std::vector<std::string> patterns = Grep::readPatternsFromFile(patternFile);
            int totalCount = 0;
            for (const auto& pat : patterns) {
                grep.setPattern(pat);
                totalCount += grep.countMatches();
            }
            std::cout << totalCount << std::endl;
        } else {
            std::cout << grep.countMatches() << std::endl;
        }
    } else {
        if (usePatternFile) {
            std::vector<std::string> patterns = Grep::readPatternsFromFile(patternFile);
            for (const auto& pat : patterns) {
                grep.setPattern(pat);
                grep.findMatches();
            }
        } else {
            grep.findMatches();
        }
    }
}

#endif // !HELPER_FUNCTIONS_H_