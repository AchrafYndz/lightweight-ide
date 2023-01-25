#ifndef LIGHTWEIGHT_IDE_V2_ASTREE_H
#define LIGHTWEIGHT_IDE_V2_ASTREE_H

#include "automata/Value.h"
#include "lexer.h"

#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <variant>
#include <vector>

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

template <typename T>
class ASTNode : public std::enable_shared_from_this<ASTNode<T>> {
    // ASTType type;
    std::vector<std::shared_ptr<ASTNode>> nodes;
    std::shared_ptr<T> value;

public:
    ASTNode() = default;
    ASTNode(T* value, std::vector<std::shared_ptr<ASTNode>> nodes);
    inline ASTNode(std::shared_ptr<T> value, std::vector<std::shared_ptr<ASTNode>> nodes) {
        this->value = value;
        this->nodes = nodes;
    };

    const std::shared_ptr<T> getValue() const;
    void setValue(T* value);

    const std::vector<std::shared_ptr<ASTNode>>& getNodes();
    void setNodes(std::vector<std::shared_ptr<ASTNode>> nodes);

    void inorderVisit(std::shared_ptr<ASTNode<std::variant<std::string, Lexer::NextToken>>> parentNode,
                      nlohmann::json& json);
};

template <typename T>
class ASTree {
    std::shared_ptr<ASTNode<T>> root;

public:
    ASTree() = default;
    explicit ASTree(ASTNode<T>* root);
    inline ASTree(std::shared_ptr<ASTNode<T>> root) { this->root = root; };

    std::shared_ptr<ASTNode<T>> getRoot();
    void setRoot(ASTNode<T>* root);

    void printTree();
    std::string getContent();
    void getContent(std::string& content_, std::shared_ptr<ASTNode<T>> rootNode, std::vector<bool>& fList, int d,
                    bool l);

    int getHeight(std::shared_ptr<ASTNode<T>> node);
};

template <typename T>
ASTree<T>::ASTree(ASTNode<T>* root) {
    ASTree::root = std::shared_ptr<ASTNode<T>>(root);
}

template <typename T>
int ASTree<T>::getHeight(std::shared_ptr<ASTNode<T>> node) {
    if (node == nullptr || node->getNodes().size() == 0)
        return 0;
    int height = 0;
    for (auto node_ : node->getNodes())
        height = std::max(height, getHeight(node_));
    return ++height;
}

template <typename T>
void ASTree<T>::printTree() {
    int height = getHeight(root);
    height = (height == 0) ? 1 : height;
    std::vector<bool> flag(height, true);
    std::string content_ = "";
    getContent(content_, root, flag, 0, false);
    std::cout << content_ << std::endl;
}

template <typename T>
std::string ASTree<T>::getContent() {
    int height = getHeight(root);
    height = (height == 0) ? 1 : height;
    std::vector<bool> flag(height, true);
    std::string content_ = "";
    getContent(content_, root, flag, 0, false);
    return content_;
}

template <typename T>
void ASTree<T>::getContent(std::string& content_, std::shared_ptr<ASTNode<T>> rootNode, std::vector<bool>& fList, int d,
                           bool l) {
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
        content_ += *rootNode->getValue() + "\n";
    } else if (l) {
        content_ += "+--- " + *rootNode->getValue() + "\n";
        fList[d] = false;
    } else {
        content_ += "+--- " + *rootNode->getValue() + "\n";
    }
    for (const auto& node : rootNode->getNodes())
        getContent(content_, node, fList, d + 1, (0 == (rootNode->getNodes().size()) - 1));
    fList[d] = true;
}

template <>
inline void ASTree<Value>::getContent(std::string& content_, std::shared_ptr<ASTNode<Value>> rootNode,
                                      std::vector<bool>& fList, int d, bool l) {
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
    } else if (l) {
        content_ += "+--- " + rootNode->getValue()->getName() + "\n";
        fList[d] = false;
    } else {
        content_ += "+--- " + rootNode->getValue()->getName() + "\n";
    }
    for (const auto& node : rootNode->getNodes())
        getContent(content_, node, fList, d + 1, (0 == (rootNode->getNodes().size()) - 1));
    fList[d] = true;
}

template <typename T>
std::shared_ptr<ASTNode<T>> ASTree<T>::getRoot() {
    return root;
}

template <typename T>
void ASTree<T>::setRoot(ASTNode<T>* root) {
    ASTree::root = std::shared_ptr<ASTNode<T>>(root);
}

template <typename T>
ASTNode<T>::ASTNode(T* value, std::vector<std::shared_ptr<ASTNode<T>>> nodes) {
    ASTNode::value = std::shared_ptr<T>(value);
    ASTNode::nodes = nodes;
}

template <typename T>
const std::shared_ptr<T> ASTNode<T>::getValue() const {
    return value;
}

template <typename T>
void ASTNode<T>::setValue(T* value) {
    ASTNode::value = std::shared_ptr<T>(value);
}

template <typename T>
const std::vector<std::shared_ptr<ASTNode<T>>>& ASTNode<T>::getNodes() {
    return nodes;
}

template <typename T>
void ASTNode<T>::setNodes(std::vector<std::shared_ptr<ASTNode<T>>> nodes) {
    ASTNode::nodes = nodes;
}

// inorder traverse while keeping track of parent
template <>
inline void ASTNode<std::variant<std::string, Lexer::NextToken>>::inorderVisit(
    std::shared_ptr<ASTNode<std::variant<std::string, Lexer::NextToken>>> parentNode, nlohmann::json& json) {
    // Check if leaf
    if (getNodes().empty()) {
        std::shared_ptr<std::variant<std::string, Lexer::NextToken>> value_ = getValue();
        std::string str = std::get<Lexer::NextToken>(*value_).value;
        std::string type = std::get<std::string>(*parentNode->getValue());
        Lexer::TokenType self_type = std::get<Lexer::NextToken>(*value_).type;
        //        std::cout << "Found leaf: `" << str << "` with parent " << type << " and type " << int(self_type) <<
        //        std::endl;

        std::pair<unsigned int, unsigned int> start = std::get<Lexer::NextToken>(*value_).start;
        std::pair<unsigned int, unsigned int> end = std::get<Lexer::NextToken>(*value_).end;
        //        std::cout << "|___start: {" << start.first << ", " << start.second << "} , end: {" << end.first << ",
        //        "
        //                  << end.second << "}" << std::endl;

        std::map<std::string, std::pair<unsigned int, unsigned int>> json_key = {{"start", start}, {"end", end}};

        switch (self_type) {
        case Lexer::TokenType::Comment:
            json["comment"] += json_key;
            break;
        case Lexer::TokenType::Keyword:
            json["keyword"] += json_key;
            break;
        case Lexer::TokenType::Literal:
            json["literal"] += json_key;
            break;
        case Lexer::TokenType::Identifier:
            if (type == "<function>") {
                json["function_name"] += json_key;
            } else if (type == "<function_call>") {
                json["function_call_identifier"] += json_key;
            } else if (type == "<arguments>") {
                json["argument"] += json_key;
            } else
                json["identifier"] += json_key;
            break;
        default:
            break;
        }

        return;
    }

    // Visit children
    for (const auto& child : getNodes()) {
        //        std::shared_ptr<std::variant<std::string, Lexer::NextToken>> value_ = getValue();
        //        std::cout << "Visiting child of " << std::get<std::string>(*value_) << std::endl;
        child->inorderVisit(this->shared_from_this(), json);
    }

    // Visit current node
    //    std::shared_ptr<std::variant<std::string, Lexer::NextToken>> value_ = getValue();
    //    std::cout << "Visiting: " << std::get<std::string>(*value_) << std::endl;
}

#endif // LIGHTWEIGHT_IDE_V2_ASTREE_H
