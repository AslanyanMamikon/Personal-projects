#ifndef REGEX_H__
#define REGEX_H__

#include <stack>
#include <map>
#include <set>
#include "ASTNode.h"
#include "DFAState.h"
#include "RegexStream.h"

class Regex {
private:
    std::map<int, ASTNode*>  treeMap;
    std::map<int, bool>      charsMap;
    std::map<int, DFAState*> stateMap;

    ASTNode*  root;
    DFAState* initialState; // tracked by pointer, not hardcoded index

    // O(log n) DFA state deduplication (replaces old O(n) linear scan)
    std::map<std::set<ASTNode*>, DFAState*> setToState;

private:
    // --- Recursive-descent parser ---
    // Grammar:
    //   expr   -> term ( '|' term )*
    //   term   -> factor factor*
    //   factor -> atom '*'*
    //   atom   -> CHAR | '(' expr ')'
    ASTNode* parseExpr  (RegexStream* stream);
    ASTNode* parseTerm  (RegexStream* stream);
    ASTNode* parseFactor(RegexStream* stream);
    ASTNode* parseAtom  (RegexStream* stream);

    // --- Node factory methods ---
    ASTNode* makeConcat(ASTNode* left, ASTNode* right);
    ASTNode* makeOr    (ASTNode* left, ASTNode* right);
    ASTNode* makeStar  (ASTNode* child);

    // --- Existing helpers ---
    ASTNode* NewCharNode(int c);
    void     AddTree(ASTNode* tree);
    ASTNode* ConstructTree(const char* str);
    bool     ConstructDFA();
    void     Cleanup();

public:
    Regex() : root(nullptr), initialState(nullptr), setToState() { }
    ~Regex() { Cleanup(); }

    bool Compile(const char* str);
    bool Match  (const char* str);
};

#endif  // REGEX_H__