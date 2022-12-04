#include "lr.h"

#include <cassert>
#include <queue>

LR::LR(const CFG& cfg) {
    // check that the augmented start variable is not an existing variable
    const std::string aug_start_var("S'");
    if (cfg.get_vars().find(aug_start_var) != cfg.get_vars().end())
        throw "augmented start variable connot be an existing variable";

    // check that the seperator symbol is not a used variable/terminal
    const std::string separator("·");
    if (cfg.get_vars().find(separator) != cfg.get_vars().end() &&
        cfg.get_terms().find(separator.at(0)) != cfg.get_terms().end())
        throw "seperator is not an existing terminal or variable";

    // keep track of all produced items
    // Note: this cannot be a set, because an ordering using indices needs to be kept.
    //       However, any `Item` in the vector should be unique.
    std::vector<ItemSet> itemsets{};
    std::map<unsigned int, std::set<unsigned int>> transitions{};

    // keep track of items that need to be processed
    std::queue<ItemSet> pending_items{};

    // add starting item to queue
    pending_items.push(this->closure(ItemSet{{aug_start_var, {separator, cfg.get_start_var()}}}, separator, cfg));

    while (!pending_items.empty()) {
        const ItemSet& item_set = pending_items.front();

        // processes pending items

        pending_items.pop();
    }
}

LR::ItemSet LR::closure(const ItemSet& item, const std::string& separator, const CFG& cfg) const {
    ItemSet result(item);

    bool changed{true};
    while (changed) {
        changed = false;

        for (const Item& item : result) {
            // find seperator in item
            auto it = std::find(item.second.begin(), item.second.end(), separator);

            // DEBUG: check that the seperator is found
            assert((it != item.second.end() && "seperaotr is found in item"));

            // check that iterator is in the body and that the found value is not a terminal
            std::advance(it, 1);
            if (it == item.second.end() ||
                (it->length() == 1 && cfg.get_terms().find(it->at(0)) != cfg.get_terms().end()))
                continue;

            const std::set<CFG::Body>& rule = cfg.get_rules().at(*it);

            for (const CFG::Body& body : rule) {
                const Item new_item{*it, [&]() {
                                        std::vector tmp(body);
                                        tmp.insert(tmp.begin(), separator);
                                        return tmp;
                                    }()};

                // check that the item is not present in itemset
                if (result.find(new_item) != result.end())
                    continue;

                result.insert(new_item);
                changed = true;
            }
        }
    }

    return result;
}
