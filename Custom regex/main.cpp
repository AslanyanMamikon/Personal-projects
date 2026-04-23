#include "Regex.h"
#include <iostream>
#include <string>

void test_regex(const char* pattern, const char* test_str, bool expected) {
    std::cout << "\n========================================\n";
    std::cout << "Testing pattern: \"" << pattern << "\" with string: \"" << test_str << "\"\n";
    std::cout << "Expected result: " << (expected ? "Match" : "No match") << "\n";
    std::cout << "========================================\n";
    
    Regex regex;
    if (regex.Compile(pattern)) {
        bool result = regex.Match(test_str);
        if (result == expected) {
            std::cout << "Test passed! ";
        } else {
            std::cout << "Test failed! ";
        }
        std::cout << (result ? "Match found" : "No match found") << "\n";
    } else {
        std::cout << "Failed to compile pattern.\n";
    }
}

int main() {
    std::cout << "Custom Regex Engine Test Suite\n";
    std::cout << "========================================\n\n";

    // Test 1: Basic character matching
    std::cout << "\nTest Group 1: Basic Character Matching\n";
    test_regex("a", "a", true);
    test_regex("a", "b", false);
    test_regex("a", "aa", false);
    test_regex("a", "", false);

    // Test 2: Concatenation
    std::cout << "\nTest Group 2: Concatenation\n";
    test_regex("ab", "ab", true);                                   // Test failed
    test_regex("ab", "a", false);
    test_regex("ab", "b", false);                                   // Test failed
    test_regex("ab", "abc", false);

    // Test 3: Alternation (|)
    std::cout << "\nTest Group 3: Alternation\n";
    test_regex("a|b", "a", true);                                   // Test failed
    test_regex("a|b", "b", true);
    test_regex("a|b", "c", false);
    test_regex("a|b", "ab", false);

    // Test 4: Kleene Star (*)
    std::cout << "\nTest Group 4: Kleene Star\n";
    test_regex("a*", "", true);                                     // Test failed
    test_regex("a*", "a", true);
    test_regex("a*", "aa", true);
    test_regex("a*", "aaa", true);
    test_regex("a*", "b", false);

    // Test 5: Grouping
    std::cout << "\nTest Group 5: Grouping\n";
    test_regex("(ab)", "ab", true);                                 // Test failed
    test_regex("(ab)", "a", false);
    test_regex("(a|b)*", "abab", true);                             // Test failed
    test_regex("(a|b)*", "aaaa", true);                             // Test failed
    test_regex("(a|b)*", "c", false);

    // Test 6: Complex Patterns
    std::cout << "\nTest Group 6: Complex Patterns\n";
    test_regex("a(b|c)*d", "abd", true);                            // Test failed
    test_regex("a(b|c)*d", "acd", true);                            // Test failed
    test_regex("a(b|c)*d", "abbcd", true);                          // Test failed
    test_regex("a(b|c)*d", "ab", false);
    test_regex("a(b|c)*d", "ad", true);                             // Test failed
    test_regex("a(b|c)*d", "abc", false);

    // Test 7: Empty Patterns
    std::cout << "\nTest Group 7: Empty Patterns\n";
    test_regex("", "", true);                                       // Failed to compile pattern.
    test_regex("", "a", false);                                     // Failed to compile pattern.

    // Test 8: Multiple Operators
    std::cout << "\nTest Group 8: Multiple Operators\n";
    test_regex("(a|b)*c", "c", true);
    test_regex("(a|b)*c", "ac", true);                              // Test failed
    test_regex("(a|b)*c", "bc", true);                              // Test failed
    test_regex("(a|b)*c", "abc", true);                             // Test failed
    test_regex("(a|b)*c", "ab", false);

    return 0;
}