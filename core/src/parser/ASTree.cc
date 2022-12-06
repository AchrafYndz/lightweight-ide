//
// Created by maxime on 6/12/22.
//

#include "ASTree.h"
#include <iostream>

ASTree::ASTree(ASTNode* root) {
    ASTree::root = root;
}

void ASTree::printTree() {
    ASTree::print(root);
    std::cout << std::endl;
}

void ASTree::print(ASTNode* node) {
    if(node == nullptr)
        return;
    ASTree::print(node->getLeftNode());
    if(node->getContent() != "")
        std::cout << node->getContent() << " ";
    ASTree::print(node->getRightNode());
}

ASTNode* ASTree::getRoot() {
    return root;
}

void ASTree::setRoot(ASTNode* root) {
    ASTree::root = root;
}

std::string ASTree::getContentTree() {
    std::string content_;
    ASTree::getContent(root, content_);
    return content_;
}

void ASTree::getContent(ASTNode* node, std::string content_) {
    if(node == nullptr)
        return;
    ASTree::getContent(node->getLeftNode(), content_);
    if(node->getContent() != "")
        content_ += node->getContent() + " ";
    ASTree::getContent(node->getRightNode(), content_);
}

ASTNode::ASTNode(ASTType type, ASTNode* leftNode, ASTNode* rightNode) {
    ASTNode::type = type;
    ASTNode::leftNode = leftNode;
    ASTNode::rightNode = rightNode;
}

ASTNode::ASTNode(ASTType type, ASTNode* leftNode, std::string content, ASTNode* rightNode) {
    ASTNode::type = type;
    ASTNode::leftNode = leftNode;
    ASTNode::content = content;
    ASTNode::rightNode = rightNode;
}

ASTType ASTNode::getType(){
    return ASTNode::type;
}

void ASTNode::setType(ASTType type) {
    ASTNode::type = type;
}

ASTNode* ASTNode::getLeftNode() {
    return leftNode;
}

void ASTNode::setLeftNode(ASTNode* leftNode) {
    ASTNode::leftNode = leftNode;
}

std::string ASTNode::getContent() {
    return content;
}

void ASTNode::setContent(std::string content) {
    ASTNode::content = content;
}

ASTNode* ASTNode::getRightNode() {
    return rightNode;
}

void ASTNode::setRightNode(ASTNode* rightNode) {
    ASTNode::rightNode = rightNode;
}

