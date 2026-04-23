#include "Grep.h"
#include <filesystem>

bool Grep::isWildcardPattern(const std::string& pattern) {
	return pattern.find('*') != std::string::npos;
}

bool Grep::matchesWildcard(const std::string& filename, const std::string& pattern) {
	std::string regexPattern;
	for (char ch : pattern) {
		if (ch == '*') {
			regexPattern += ".*";
		} else if (std::string(".^$()[]{}+?|\\").find(ch) != std::string::npos) {
			regexPattern += "\\" + std::string(1, ch);
		} else {
			regexPattern += ch;
		}
	}
	return std::regex_match(filename, std::regex(regexPattern));
}

std::vector<std::string> Grep::expandWildcards(const std::string& pattern) {
	std::vector<std::string> matches;

	if (!isWildcardPattern(pattern)) {
		matches.push_back(pattern);
		return matches;
	}

	namespace fs = std::filesystem;

	size_t lastSlash = pattern.find_last_of("/\\");
	std::string directory = ".";
	std::string filenamePattern = pattern;

	if (lastSlash != std::string::npos) {
		directory = pattern.substr(0, lastSlash);
		filenamePattern = pattern.substr(lastSlash + 1);
	}

	try {
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (!entry.is_regular_file()) continue;

			const std::string filename = entry.path().filename().string();

			if (matchesWildcard(filename, filenamePattern)) {
				matches.push_back(entry.path().string());
			}
		}
	} catch (const fs::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << "\n";
	}

	return matches;
}

void Grep::findMatches() {
	std::vector<std::string> files = expandWildcards(filename);
	bool wildcardExpanded = isWildcardPattern(filename);
	bool anyFound = false;

	for (const auto& file : files) {
		std::ifstream inputFile(file);
		if (!inputFile.is_open()) {
			std::cerr << "Error: Cannot open file '" << file << "'\n";
			continue;
		}

		std::regex re = createRegex();
		std::string line;
		int lineNumber = 1;

		while (std::getline(inputFile, line)) {
			if (isMatch(line, re)) {
				anyFound = true;
				std::string highlighted = highlightMatch(line);
				if (showLineNumbers) {
					if (hideFilename) {
						std::cout << lineNumber << " : " << highlighted << "\n";
					} else {
						std::cout << file << " : " << lineNumber << " : " << highlighted << "\n";
					}
				} else {
					if (hideFilename) {
						std::cout << highlighted << "\n";
					} else {
						std::cout << file << " : " << highlighted << "\n";
					}
				}
			}
			lineNumber++;
		}

		inputFile.close();
	}

	// Only report "not found" for an explicit single file, not for wildcard
	// expansions (where zero matches across many files is normal and not an error).
	if (!anyFound && !wildcardExpanded) {
		if (files.empty()) {
			std::cerr << "Error: No files found for '" << filename << "'\n";
		} else {
			std::cout << "Pattern '" << pattern << "' not found in file '" << filename << "'\n";
		}
	}
}

int Grep::countMatches() {
	std::vector<std::string> files = expandWildcards(filename);
	int totalCount = 0;

	for (const auto& file : files) {
		std::ifstream inputFile(file);
		if (!inputFile.is_open()) {
			std::cerr << "Error: Cannot open file '" << file << "'\n";
			continue;
		}

		std::regex re = createRegex();
		std::string line;
		int count = 0;

		while (std::getline(inputFile, line)) {
			if (isMatch(line, re)) {
				count++;
			}
		}

		totalCount += count;
		inputFile.close();
	}

	return totalCount;
}

std::regex Grep::createRegex() const {
	// In extended-regex mode the pattern is a proper ERE — pass it straight
	// through to std::regex (which uses ECMAScript / ERE syntax) so that
	// anchors (^/$), groups, alternation (|), and all standard quantifiers
	// (+, ?, {n,m}) work correctly.
	//
	// In basic mode the pattern is treated as a literal fixed string that is
	// searched as a substring, which matches the classic non-regex grep BRE
	// default behaviour.  We escape every regex meta-character so that chars
	// like '.', '*', '(', etc. are matched literally.
	std::string regexPattern;

	if (extendedRegex) {
		// Use the pattern as-is; std::regex understands ERE syntax natively.
		regexPattern = pattern;
	} else {
		// Escape all regex metacharacters so the pattern is a literal search.
		static const std::string META = R"(\.^$|?+*()[]{})";
		for (char ch : pattern) {
			if (META.find(ch) != std::string::npos) {
				regexPattern += '\\';
			}
			regexPattern += ch;
		}
	}

	auto flags = std::regex::ECMAScript;
	if (caseInsensitive) flags |= std::regex::icase;
	return std::regex(regexPattern, flags);
}

bool Grep::isMatch(const std::string& line, const std::regex& re) const {
	bool matches = std::regex_search(line, re);
	return invertMatch ? !matches : matches;
}

std::string Grep::highlightMatch(const std::string& line) const {
	std::regex re = createRegex();

	// ANSI color codes
	const std::string RED = "\033[31m";
	const std::string RESET = "\033[0m";

	// Build result by appending: plain segment, RED, match, RESET, repeat.
	// This avoids insert()-based offset corruption when multiple matches exist.
	std::string result;
	std::smatch match;
	std::string::const_iterator searchStart(line.cbegin());

	while (std::regex_search(searchStart, line.cend(), match, re)) {
		// Append the non-matching text before this match
		result.append(match.prefix().first, match.prefix().second);
		// Wrap the matched text in color codes
		result += RED;
		result += match[0].str();
		result += RESET;
		// Advance past the match; guard against zero-length matches
		if (match[0].length() == 0) {
			if (searchStart == line.cend()) break;
			result += *searchStart;
			++searchStart;
		} else {
			searchStart = match[0].second;
		}
	}
	// Append any remaining text after the last match
	result.append(searchStart, line.cend());

	return result;
}

std::vector<std::string> Grep::readPatternsFromFile(const std::string& patternFile) {
	std::vector<std::string> patterns;
	std::ifstream file(patternFile);
	
	if (!file.is_open()) {
		std::cerr << "Error: Cannot open pattern file '" << patternFile << "'\n";
		return patterns;
	}

	std::string line;
	while (std::getline(file, line)) {
		// Strip carriage return for Windows-style CRLF files
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (!line.empty()) {
			patterns.push_back(line);
		}
	}

	file.close();
	return patterns;
}