#ifndef GREP_H_
#define GREP_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

class Grep {
private:
	std::string filename;
	std::string pattern;
	bool caseInsensitive;
	bool invertMatch;
	bool showLineNumbers;
	bool countOnly;
	bool hideFilename;
	bool extendedRegex;

public:
	Grep(const std::string& filename, const std::string& pattern) 
		: filename(filename), pattern(pattern), 
		  caseInsensitive(false), invertMatch(false),
		  showLineNumbers(false), countOnly(false),
		  hideFilename(false), extendedRegex(false) {}
	
	~Grep() = default;

	// Prints matching lines
	void findMatches();
	// Returns vector of matching lines without line numbers
	//std::vector<std::string> findMatches();
	int countMatches();
	
	std::string highlightMatch(const std::string& line) const;

	static std::vector<std::string> readPatternsFromFile(const std::string& patternFile);
	
	void setCaseInsensitive(bool value) { caseInsensitive = value; }
	void setInvertMatch(bool value) { invertMatch = value; }
	void setShowLineNumbers(bool value) { showLineNumbers = value; }
	void setCountOnly(bool value) { countOnly = value; }
	void setHideFilename(bool value) { hideFilename = value; }
	void setExtendedRegex(bool value) { extendedRegex = value; }

	const std::string& getFilename() const { return filename; }
	const std::string& getPattern() const { return pattern; }
	void setFilename(const std::string& newFilename) { filename = newFilename; }
	void setPattern(const std::string& newPattern) { pattern = newPattern; }

private:
	std::regex createRegex() const;
	bool isMatch(const std::string& line, const std::regex& re) const;
	std::vector<std::string> expandWildcards(const std::string& pattern);
	bool isWildcardPattern(const std::string& pattern);
	bool matchesWildcard(const std::string& filename, const std::string& pattern);
};

#endif // !GREP_H_