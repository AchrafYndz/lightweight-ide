//
// Created by maxime on 6/12/22.
//

#include "../automata/Value.h"
#include <string>
#include <vector>
#include <memory>

#ifndef LIGHTWEIGHT_IDE_V2_ASTREE_H
#define LIGHTWEIGHT_IDE_V2_ASTREE_H

/*enum ASTType {
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
};*/

class ASTNode {
    // ASTType type;
    std::vector<std::shared_ptr<ASTNode>> nodes;
    std::shared_ptr<Value> value;

public:
    ASTNode() = default;
    ASTNode(Value* value, std::vector<std::shared_ptr<ASTNode>> nodes);

    const std::shared_ptr<Value> getValue() const;
    void setValue(Value* value);

    std::vector<std::shared_ptr<ASTNode>> getNodes();
    void setNodes(std::vector<std::shared_ptr<ASTNode>> nodes);
};

class ASTree {
    std::shared_ptr<ASTNode> root;

public:
    ASTree() = default;
    ASTree(ASTNode* root);

    void printTree();
    void print(std::vector<std::shared_ptr<ASTNode>> nodes);

    std::string getContentTree();
    void getContent(std::vector<std::shared_ptr<ASTNode>> nodes, std::string &content_);

    std::shared_ptr<ASTNode> getRoot();
    void setRoot(ASTNode* root);
};

#endif // LIGHTWEIGHT_IDE_V2_ASTREE_H