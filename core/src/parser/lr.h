#ifndef IDE_SRC_PARSERS_LR_H
#define IDE_SRC_PARSERS_LR_H

#include "../machines/CFG.h"

#include <map>
#include <stdexcept>

class LR {
public:
    using Item = std::pair<CFG::Var, CFG::Body>;
    using ItemSet = std::set<Item>;

public:
    class LRException : public std::exception {};

    class LRCFGParsingException : public LRException {
    public:
        enum Type {
            AugStartVarNotValid,
            SeparatorSymbNotValid,
        };

    public:
        inline LRCFGParsingException(Type type) : type(type) {}

        inline const char* what() const noexcept override {
            switch (this->type) {
            case AugStartVarNotValid:
                return "augmented start variable connot be an existing variable";
            case SeparatorSymbNotValid:
                return "separator is not an existing terminal or variable";
            }
        }

    private:
        const Type type{};
    };

public:
    LR(const CFG& cfg);

private:
    static ItemSet closure(const ItemSet& item, const std::string& separator, const CFG& cfg);

private:
    // ParsingTable table{};

#ifdef TEST
private:
    friend class LRTest;
#endif
};

#endif // IDE_SRC_PARSERS_LR_H
