#include "../automata/Value.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <queue>

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

template <typename Value>
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

template <typename Value>
class ASTree {
    std::shared_ptr<ASTNode<Value>> root;

public:
    ASTree() = default;
    ASTree(ASTNode<Value>* root);

    std::shared_ptr<ASTNode<Value>> getRoot();
    void setRoot(ASTNode<Value>* root);

    void printTree();
    std::string getContent();
    void getContent(std::string &content_, std::shared_ptr<ASTNode<Value>> rootNode, std::vector<bool> fList, int d, bool l);

};

template <typename Value>
ASTree<Value>::ASTree(ASTNode<Value>* root) {
    ASTree::root = std::shared_ptr<ASTNode<Value>>(root);
}

template <typename Value>
void ASTree<Value>::printTree() {
    std::vector<bool> flag(10, true);
    std::string content_ = "";
    getContent(content_, root, flag, 0, false);
    std::cout << content_ << std::endl;
}


template<typename Value>
std::string ASTree<Value>::getContent() {
    std::vector<bool> flag(10, true);
    std::string content_ = "";
    getContent(content_, root, flag, 0, false);
    return content_;
}

template <typename Value>
void ASTree<Value>::getContent(std::string &content_, std::shared_ptr<ASTNode<Value>> rootNode, std::vector<bool> fList, int d, bool l){
    if (rootNode == nullptr)
        return;
    for (int i = 1; i < d; i++) {
        if (fList[i]) {
            content_ += "|    ";
            continue;
        }
        content_ += "     ";
    }
    if (d == 0) {
        content_ += rootNode->getValue()->getName() + "\n";
    }
    else if (l) {
        content_ += "+--- " + rootNode->getValue()->getName() + "\n";
        fList[d] = false;
    }
    else {
        content_ += "+--- " + rootNode->getValue()->getName() + "\n";
    }
    for(const auto &node: rootNode->getNodes())
        getContent(content_, node, fList, d + 1,(0 == (rootNode->getNodes().size()) - 1));
    fList[d] = true;
}

template <typename Value>
std::shared_ptr<ASTNode<Value>> ASTree<Value>::getRoot() { return root; }

template <typename Value>
void ASTree<Value>::setRoot(ASTNode<Value>* root) {
    ASTree::root = std::shared_ptr<ASTNode<Value>>(root);
}

template <typename Value>
ASTNode<Value>::ASTNode(Value* value, std::vector<std::shared_ptr<ASTNode<Value>>> nodes) {
    ASTNode::value = std::shared_ptr<Value>(value);
    ASTNode::nodes = nodes;
}

template <typename Value>
const std::shared_ptr<Value> ASTNode<Value>::getValue() const { return value; }

template <typename Value>
void ASTNode<Value>::setValue(Value* value) {
    ASTNode::value = std::shared_ptr<Value>(value);
}

template <typename Value>
std::vector<std::shared_ptr<ASTNode<Value>>> ASTNode<Value>::getNodes() { return nodes; }

template <typename Value>
void ASTNode<Value>::setNodes(std::vector<std::shared_ptr<ASTNode<Value>>> nodes) { ASTNode::nodes = nodes; }

#endif // LIGHTWEIGHT_IDE_V2_ASTREE_H