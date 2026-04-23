#include "ASTNode.h"

int ASTNode::globalCounter = 0;

int ASTNode::get_index() const { 
    return index; 
}

ASTNodeType ASTNode::get_type() const { 
    return type; 
}

int ASTNode::get_char() const { 
    return c; 
}

ASTNode* ASTNode::get_left() const { 
    return left; 
}

ASTNode* ASTNode::get_right() const { 
    return right; 
}

bool ASTNode::get_nullable() const { 
    return nullable; 
}

const std::set<ASTNode*>& ASTNode::get_firstpos() const { 
    return firstpos; 
}

const std::set<ASTNode*>& ASTNode::get_lastpos() const { 
    return lastpos; 
}

const std::set<ASTNode*>& ASTNode::get_follow_pos() const { 
    return followpos; 
}

void ASTNode::set_left(ASTNode* node) { 
    left = node; 
}

void ASTNode::set_right(ASTNode* node) { 
    right = node; 
}

void ASTNode::set_nullable(bool value) { 
    nullable = value; 
}

void ASTNode::add_firstpos(const std::set<ASTNode*>& nodes) {
    firstpos.insert(nodes.begin(), nodes.end());
}

void ASTNode::add_lastpos(const std::set<ASTNode*>& nodes) {
    lastpos.insert(nodes.begin(), nodes.end());
}

void ASTNode::add_followpos(const std::set<ASTNode*>& nodes) {
    followpos.insert(nodes.begin(), nodes.end());
}

void ASTNode::set_parent(ASTNode* new_parent) {
    parent = new_parent;
}
