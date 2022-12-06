//
// Created by maxime on 6/12/22.
//

#include <string>

#ifndef LIGHTWEIGHT_IDE_V2_ASTREE_H
#define LIGHTWEIGHT_IDE_V2_ASTREE_H

enum ASTType {
    import_declaration,
    import_identifier,

    function_declaration,
    function_header,
    function_identifier,
    function_function_arguments,
    function_body,

    while_block,
    while_body,

    if_else_block,
    if_block,
    elif_block,
    else_bock,
    if_body,
    elif_body,
    else_body,

    return_statement,

    expression,
    equal_statement,
    identifier,
    value,
    literal,
    multiplication,
    subtraction,
    addition,
    division,
    function_call,
    arguments,
};

class ASTNode {
    ASTType type;
    ASTNode* leftNode;
    std::string content;
    ASTNode* rightNode;
public:
    ASTNode() = default;
    ASTNode(ASTType type, ASTNode* leftNode, std::string content, ASTNode* rightNode);
    ASTNode(ASTType type, ASTNode* leftNode, ASTNode* rightNode);

    ASTType getType();
    void setType(ASTType type);

    ASTNode* getLeftNode();
    void setLeftNode(ASTNode* leftNode);

    std::string getContent();
    void setContent(std::string content);

    ASTNode* getRightNode();
    void setRightNode(ASTNode* rightNode);
};

class ASTree {
    ASTNode* root;
public:
    ASTree() = default;
    ASTree(ASTNode* root);

    void printTree();
    void print(ASTNode* node);

    std::string getContentTree();
    void getContent(ASTNode* node, std::string content_);

    ASTNode* getRoot();
    void setRoot(ASTNode* root);
};


#endif //LIGHTWEIGHT_IDE_V2_ASTREE_H
