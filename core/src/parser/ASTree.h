//
// Created by maxime on 6/12/22.
//

#include <string>
#include <vector>
#include "../automata/Value.h"

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
    //ASTType type;
    std::vector<ASTNode*> nodes;
    Value* value;
public:
    ASTNode() = default;
    ASTNode(Value* value, std::vector<ASTNode*> nodes);

    Value* getValue();
    void setValue(Value* value);

    std::vector<ASTNode*> getNodes();
    void setNodes(std::vector<ASTNode*> nodes);
};

class ASTree {
    ASTNode* root;
public:
    ASTree() = default;
    ASTree(ASTNode* root);

    void printTree();
    void print(std::vector<ASTNode*> nodes);

    std::string getContentTree();
    void getContent(std::vector<ASTNode*> nodes, std::string content_);

    ASTNode* getRoot();
    void setRoot(ASTNode* root);
};


#endif //LIGHTWEIGHT_IDE_V2_ASTREE_H
