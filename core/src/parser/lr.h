#ifndef IDE_SRC_PARSERS_LR_H
#define IDE_SRC_PARSERS_LR_H

#include "../machines/CFG.h"

#include <map>

class LR {
public:
    using Item = std::pair<std::string, CFG::Body>;
    using ItemSet = std::set<Item>;

public:
    LR(const CFG& cfg);

private:
    ItemSet closure(const ItemSet& item, const std::string& separator, const CFG& cfg) const;

private:
    // ParsingTable table{};

#ifdef TEST
private:
    friend class LRTest;
#endif
};

#endif // IDE_SRC_PARSERS_LR_H
