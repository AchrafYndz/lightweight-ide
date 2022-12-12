//
// Created by maxime on 6/12/22.
//

#include "ASTree.h"
#include <iostream>

ASTree::ASTree(ASTNode* root) {
    ASTree::root = std::shared_ptr<ASTNode>(root);
}

void ASTree::printTree() {
    ASTree::print(root->getNodes());
    std::cout << std::endl;
}

void ASTree::print(std::vector<std::shared_ptr<ASTNode>> nodes) {
    if (nodes.empty())
        return;
    for (auto node : nodes) {
        if (node->getNodes().empty())
            std::cout << node->getValue()->getName() + " ";
        ASTree::print(node->getNodes());
    }
}

std::shared_ptr<ASTNode> ASTree::getRoot() { return root; }

void ASTree::setRoot(ASTNode* root) {
    ASTree::root = std::shared_ptr<ASTNode>(root);
}

std::string ASTree::getContentTree() {
    std::string content_;
    ASTree::getContent(root->getNodes(), content_);
    return content_;
}

void ASTree::getContent(std::vector<std::shared_ptr<ASTNode>> nodes, std::string &content_) {
    if (nodes.empty())
        return;
    for (auto node : nodes) {
        if (node->getNodes().empty())
            content_ += node->getValue()->getName();
        ASTree::getContent(node->getNodes(), content_);
    }
}

ASTNode::ASTNode(Value* value, std::vector<std::shared_ptr<ASTNode>> nodes) {
    ASTNode::value = std::shared_ptr<Value>(value);
    ASTNode::nodes = nodes;
}

const std::shared_ptr<Value> ASTNode::getValue() const { return value; }

void ASTNode::setValue(Value* value) {
    ASTNode::value = std::shared_ptr<Value>(value);
}


std::vector<std::shared_ptr<ASTNode>> ASTNode::getNodes() { return nodes; }

void ASTNode::setNodes(std::vector<std::shared_ptr<ASTNode>> nodes) { ASTNode::nodes = nodes; }