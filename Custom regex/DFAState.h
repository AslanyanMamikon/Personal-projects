#ifndef DFA_STATE_H__
#define DFA_STATE_H__

#include <map>
#include <set>
#include "ASTNode.h"

class DFAState {
private:
    static int globalCounter;

    // Members ordered to match constructor initialiser list
    std::set<ASTNode*> treeSet;
    bool acceptable;
    int  index;

    std::map<int, DFAState*> transferMap;

public:
    DFAState(const std::set<ASTNode*>& tree_set);
    ~DFAState() { }

    const std::set<ASTNode*>& get_tree_set() const;
    bool      get_acceptable() const;
    int       get_index() const;
    DFAState* get_transition_state(int c);
    void      AddTransferState(int c, DFAState* state);
};

#endif // DFA_STATE_H__