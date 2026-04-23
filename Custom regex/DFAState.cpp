#include "DFAState.h"

int DFAState::globalCounter = 0;

DFAState::DFAState(const std::set<ASTNode*>& tree_set)
    : treeSet(tree_set), acceptable(false), index(globalCounter++) {

    for (ASTNode* tree : treeSet) {
        if (tree->get_type() == END) {
            acceptable = true;
            break;
        }
    }
}

const std::set<ASTNode*>& DFAState::get_tree_set() const { return treeSet; }
bool DFAState::get_acceptable()                    const { return acceptable; }
int  DFAState::get_index()                         const { return index; }

void DFAState::AddTransferState(int c, DFAState* state) {
    transferMap[c] = state;
}

DFAState* DFAState::get_transition_state(int c) {
    // Fix: operator[] silently inserts null for missing keys — use find() instead.
    auto it = transferMap.find(c);
    return it != transferMap.end() ? it->second : nullptr;
}