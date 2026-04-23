#ifndef ASTNODE_H__
#define ASTNODE_H__

#include <iostream>
#include <set>

enum ASTNodeType {
    CHAR,
    OR,
    STAR,
    CONCAT,
    END
};
/*  a(b|c)*d

        CONCAT
       /      \
     CHAR     CONCAT
      |      /      \
      a    CONCAT   CHAR
          /      \    |
      STAR     CHAR   d
        |        |
      OR        b
     /  \
  CHAR  CHAR
    |     |
    b     c
*/
class ASTNode {
private:
    // Counter used to assign unique indices to AST nodes
    static int globalCounter;

    int index;
    ASTNodeType type;
    int c;
    ASTNode* left, * right;
    ASTNode* parent;
    bool nullable;
    
    std::set<ASTNode*> firstpos;
    std::set<ASTNode*> lastpos;
    std::set<ASTNode*> followpos;

public:
    ASTNode(ASTNodeType type, int c = -1)
        : type(type), c(c), left(nullptr), right(nullptr), parent(nullptr), nullable(false) {
        index = globalCounter++;
    }

    int get_index() const;
    ASTNodeType get_type() const;
    int get_char() const;
    ASTNode* get_left() const;
    ASTNode* get_right() const;
    bool get_nullable() const;
    const std::set<ASTNode*>& get_firstpos() const;
    const std::set<ASTNode*>& get_lastpos() const;
    const std::set<ASTNode*>& get_follow_pos() const;

    void set_left(ASTNode* node);
    void set_right(ASTNode* node);
    void set_nullable(bool value);

    void add_firstpos(const std::set<ASTNode*>& nodes);
    void add_lastpos(const std::set<ASTNode*>& nodes);
    void add_followpos(const std::set<ASTNode*>& nodes);
    void set_parent(ASTNode* parent);
};

#endif  // ASTNODE_H__ 