#include "Regex.h"
#include <iostream>
#include <vector>

// ---------------------------------------------------------------------------
// Recursive-descent parser
//
//   expr   -> term ( '|' term )*
//   term   -> factor factor*
//   factor -> atom '*'*
//   atom   -> CHAR | '(' expr ')'
//
// Replaces the old ad-hoc stack loop that never built CONCAT nodes and
// used ProcessConcatPos for OR (wrong nullable / firstpos / lastpos).
// ---------------------------------------------------------------------------

ASTNode* Regex::parseExpr(RegexStream* stream) {
    // expr -> term ( '|' term )*
    ASTNode* node = parseTerm(stream);

    while (stream->Peek() == '|') {
        stream->Read(); // consume '|'
        ASTNode* right = parseTerm(stream);
        node = makeOr(node, right);
    }

    return node;
}

ASTNode* Regex::parseTerm(RegexStream* stream) {
    // term -> factor factor*
    // Stops at ')', '|', or end-of-input.
    ASTNode* node = nullptr;

    while (true) {
        int peek = stream->Peek();
        if (peek == -1 || peek == ')' || peek == '|') break;

        ASTNode* next = parseFactor(stream);
        if (!next) break;

        node = node ? makeConcat(node, next) : next;
    }

    return node; // nullptr means epsilon (empty term)
}

ASTNode* Regex::parseFactor(RegexStream* stream) {
    // factor -> atom ('*')*
    ASTNode* node = parseAtom(stream);
    if (!node) return nullptr;

    while (stream->Peek() == '*') {
        stream->Read(); // consume '*'
        node = makeStar(node);
    }

    return node;
}

ASTNode* Regex::parseAtom(RegexStream* stream) {
    // atom -> CHAR | '(' expr ')'
    int c = stream->Peek();
    if (c == -1 || c == ')' || c == '|') return nullptr;

    stream->Read(); // consume the character

    if (c == '(') {
        ASTNode* inner = parseExpr(stream);
        if (stream->Peek() == ')') stream->Read(); // consume ')'
        return inner;
    }

    return NewCharNode(c);
}

// ---------------------------------------------------------------------------
// Node factory methods — set nullable / firstpos / lastpos / followpos
// ---------------------------------------------------------------------------

ASTNode* Regex::makeConcat(ASTNode* left, ASTNode* right) {
    ASTNode* node = new ASTNode(CONCAT);
    node->set_left(left);
    node->set_right(right);

    // nullable: both sides must be nullable
    node->set_nullable(left->get_nullable() && right->get_nullable());

    // firstpos: left's firstpos; add right's firstpos if left is nullable
    node->add_firstpos(left->get_firstpos());
    if (left->get_nullable()) node->add_firstpos(right->get_firstpos());

    // lastpos: right's lastpos; add left's lastpos if right is nullable
    node->add_lastpos(right->get_lastpos());
    if (right->get_nullable()) node->add_lastpos(left->get_lastpos());

    // followpos: every pos in left's lastpos can be followed by right's firstpos
    for (ASTNode* pos : left->get_lastpos()) {
        pos->add_followpos(right->get_firstpos());
    }

    return node;
}

ASTNode* Regex::makeOr(ASTNode* left, ASTNode* right) {
    // Handle nullptr children (e.g. "a|" or "|b") gracefully
    if (!left)  return right;
    if (!right) return left;

    ASTNode* node = new ASTNode(OR);
    node->set_left(left);
    node->set_right(right);

    // Fix: OR nullable is true if EITHER child is nullable (old code used &&)
    node->set_nullable(left->get_nullable() || right->get_nullable());

    // firstpos / lastpos: union of both children
    node->add_firstpos(left->get_firstpos());
    node->add_firstpos(right->get_firstpos());
    node->add_lastpos(left->get_lastpos());
    node->add_lastpos(right->get_lastpos());

    // OR has no followpos rule of its own.
    return node;
}

ASTNode* Regex::makeStar(ASTNode* child) {
    ASTNode* node = new ASTNode(STAR);
    node->set_left(child);
    node->set_nullable(true); // a* always matches empty string

    node->add_firstpos(child->get_firstpos());
    node->add_lastpos(child->get_lastpos());

    // followpos: every pos in lastpos loops back to firstpos
    for (ASTNode* pos : node->get_lastpos()) {
        pos->add_followpos(node->get_firstpos());
    }

    return node;
}

ASTNode* Regex::NewCharNode(int c) {
    ASTNode* node = new ASTNode(CHAR, c);
    node->set_nullable(false);
    node->add_firstpos({ node });
    node->add_lastpos({ node });
    AddTree(node);
    return node;
}

// ---------------------------------------------------------------------------
// Tree construction entry point
// ---------------------------------------------------------------------------

ASTNode* Regex::ConstructTree(const char* str) {
    RegexStream stream(str);
    ASTNode* body = parseExpr(&stream);

    // END marker: a synthetic leaf that represents acceptance
    ASTNode* end_node = new ASTNode(END);
    end_node->add_firstpos({ end_node });
    end_node->add_lastpos({ end_node });
    AddTree(end_node);

    if (!body) {
        // Empty pattern ("") — the initial DFA state will contain only
        // the END node, making it immediately accepting for the empty string.
        return end_node;
    }

    // body ## end_node  (the # concat from the Dragon Book)
    return makeConcat(body, end_node);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void Regex::AddTree(ASTNode* tree) {
    treeMap[tree->get_index()] = tree;
    if (tree->get_type() == CHAR) charsMap[tree->get_char()] = true;
}

// ---------------------------------------------------------------------------
// DFA construction (followpos algorithm)
// ---------------------------------------------------------------------------

bool Regex::ConstructDFA() {
    initialState = new DFAState(root->get_firstpos());
    stateMap[initialState->get_index()] = initialState;
    setToState[root->get_firstpos()] = initialState;

    std::vector<DFAState*> unprocessed;
    unprocessed.push_back(initialState);

    while (!unprocessed.empty()) {
        DFAState* current = unprocessed.back();
        unprocessed.pop_back();

        for (const auto& pair : charsMap) {
            int c = pair.first;
            std::set<ASTNode*> next_pos;

            for (ASTNode* pos : current->get_tree_set()) {
                if (pos->get_type() == CHAR && pos->get_char() == c) {
                    const std::set<ASTNode*>& fp = pos->get_follow_pos();
                    next_pos.insert(fp.begin(), fp.end());
                }
            }

            if (next_pos.empty()) continue;

            // Fix: O(log n) deduplication via map<set<ASTNode*>, DFAState*>
            // instead of the old O(n) linear scan over stateMap.
            DFAState* next_state = nullptr;
            auto it = setToState.find(next_pos);
            if (it != setToState.end()) {
                next_state = it->second;
            } else {
                next_state = new DFAState(next_pos);
                stateMap[next_state->get_index()] = next_state;
                setToState[next_pos] = next_state;
                unprocessed.push_back(next_state);
            }

            current->AddTransferState(c, next_state);
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------

void Regex::Cleanup() {
    for (auto& pair : treeMap)  delete pair.second;
    for (auto& pair : stateMap) delete pair.second;
    treeMap.clear();
    charsMap.clear();
    stateMap.clear();
    setToState.clear();
    root         = nullptr;
    initialState = nullptr;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool Regex::Compile(const char* str) {
    Cleanup();
    root = ConstructTree(str);
    if (!root) return false;
    return ConstructDFA();
}

bool Regex::Match(const char* str) {
    if (!initialState) return false;

    DFAState* current = initialState;
    for (int i = 0; str[i] != '\0'; i++) {
        current = current->get_transition_state(str[i]);
        if (!current) return false;
    }
    return current->get_acceptable();
}