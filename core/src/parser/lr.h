#ifndef IDE_SRC_PARSERS_LR_H
#define IDE_SRC_PARSERS_LR_H

#include "ASTree.h"
#include "lexer.h"
#include "machines/CFG.h"
#include "stream_reader.h"

#include <deque>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class LR {
public:
    using Item = std::pair<CFG::Var, CFG::Body>;
    using ItemSet = std::set<Item>;

    using ASTree = ASTree<std::string>;

public:
    enum class ActionType { Shift, Reduce, Accept };

public:
    using AcitionTable = std::map<std::string, std::pair<ActionType, unsigned int>>;
    using GotoTable = std::map<CFG::Var, unsigned int>;
    using ParsingTable = std::map<unsigned int, std::pair<AcitionTable, GotoTable>>;

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
    ASTree* parse(StreamReader in) const;

private:
    static ItemSet closure(const ItemSet& item, const std::string& separator, const CFG& cfg);

private:
    ParsingTable table{};
    std::vector<std::pair<CFG::Var, CFG::Body>> rules{};

    std::string end_of_input{'$'};

#ifdef TEST
private:
    friend class LRTest;
#endif
};

#endif // IDE_SRC_PARSERS_LR_H
