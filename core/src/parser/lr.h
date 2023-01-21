#ifndef IDE_SRC_PARSERS_LR_H
#define IDE_SRC_PARSERS_LR_H

#include "ASTree.h"
#include "lexer.h"
#include "machines/CFG.h"
#include "stream_reader.h"

#include <deque>
#include <map>
#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

template <>
inline void ASTree<std::variant<CFG::Var, Lexer::NextToken>>::getContent(
    std::string& content_, std::shared_ptr<ASTNode<std::variant<CFG::Var, Lexer::NextToken>>> rootNode,
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
        content_ += ((std::holds_alternative<Lexer::NextToken>(*rootNode->getValue()))
                         ? std::get<Lexer::NextToken>(*rootNode->getValue()).value
                         : std::get<CFG::Var>(*rootNode->getValue())) +
                    "\n";
    } else if (l) {
        content_ += "+--- " +
                    ((std::holds_alternative<Lexer::NextToken>(*rootNode->getValue()))
                         ? std::get<Lexer::NextToken>(*rootNode->getValue()).value
                         : std::get<CFG::Var>(*rootNode->getValue())) +
                    "\n";
        fList[d] = false;
    } else {
        content_ += "+--- " +
                    ((std::holds_alternative<Lexer::NextToken>(*rootNode->getValue()))
                         ? std::get<Lexer::NextToken>(*rootNode->getValue()).value
                         : std::get<CFG::Var>(*rootNode->getValue())) +
                    "\n";
    }
    for (const auto& node : rootNode->getNodes())
        getContent(content_, node, fList, d + 1, (0 == (rootNode->getNodes().size()) - 1));
    fList[d] = true;
}

class LR {
public:
    using Rule = std::pair<CFG::Var, CFG::Body>;

public:
    using Item = std::pair<CFG::Var, CFG::Body>;
    using ItemSet = std::set<Item>;

    using ASTree = ASTree<std::variant<CFG::Var, Lexer::NextToken>>;
    using ASTNode = ASTNode<std::variant<CFG::Var, Lexer::NextToken>>;

public:
    enum class ActionType { Shift, Reduce, Accept };

public:
    using ActionPair = std::pair<ActionType, unsigned int>;
    using AcitionTable = std::map<std::string, ActionPair>;
    using GotoTable = std::map<CFG::Var, unsigned int>;
    using ParsingTable = std::map<unsigned int, std::pair<AcitionTable, GotoTable>>;

private:
    /// With `std::tuple` being the comination of variable and syntax tree.
    ///
    /// With `std::string` being the actual token (see exception for "identifier" and "literal"), and `unsigned int` the
    /// state of the parser.
    ///
    /// With `std::optional` of the `std::variant` being used to define the starting pair.
    using StackContent =
        std::pair<std::optional<std::variant<Lexer::NextToken, std::tuple<CFG::Var, ASTree*>>>, unsigned int>;

private:
    enum class ErrorCorrectionAction {
        Insert,
        Delete,
    };

public:
    /// Abstract Lr exception class.
    class LRException : public std::exception {};

    class LRCFGParsingException : public LRException {
    public:
        enum Type {
            AugStartVarInvalid,
            SeparatorSymbInvalid,
            EndOfInputMarkertotInvalid,
        };

    public:
        inline LRCFGParsingException(Type type) : type(type) {}

        inline const char* what() const noexcept override {
            switch (this->type) {
            case AugStartVarInvalid:
                return "augmented start variable cannot be an existing variable";
            case SeparatorSymbInvalid:
                return "separator cannot be an existing terminal or variable";
            case EndOfInputMarkertotInvalid:
                return "end of input marker cannot be an existing terminal";
            }
        }

    private:
        const Type type{};
    };

public:
    LR() = default;
    LR(const CFG& cfg);

    /// Parses the given lexer using the
    std::pair<bool, ASTree*> parse(StreamReader in) const;

private:
    static ItemSet closure(const ItemSet& item, const std::string& separator, const CFG& cfg);

    /// Converts lexer token to a token string used in parse table.
    inline std::string lexer_token_to_parse_string(const Lexer::NextToken& token) const {
        std::string result = token.value;
        if (token.type == Lexer::TokenType::Identifier || token.type == Lexer::TokenType::Literal ||
            token.type == Lexer::TokenType::Eof) {
            result = (token.type == Lexer::TokenType::Identifier) ? "identifier"
                     : (token.type == Lexer::TokenType::Literal)  ? "literal"
                                                                  : LR::end_of_input;
        }

        return result;
    }

    bool handle_action(ActionPair action_pair, const std::string& lookup_token, const Lexer::NextToken& lexer_token,
                       std::stack<StackContent>& stack, unsigned int& parser_state) const;

    std::vector<std::pair<ErrorCorrectionAction, std::optional<std::string>>> handle_error(
        const std::string& current_lexer_token, const Lexer& lexer, unsigned int parser_state,
        std::stack<StackContent> stack) const;

private:
    ParsingTable table{};
    std::vector<Rule> rules{};

    std::string end_of_input{'$'};

#ifdef TEST
private:
    friend class LRTest;
#endif
};

#endif // IDE_SRC_PARSERS_LR_H
