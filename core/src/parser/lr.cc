#include "lr.h"

#include <cassert>
#include <queue>

LR::LR(const CFG& cfg) {
    // check that the augmented start variable is not an existing variable
    const std::string aug_start_var("S'");
    if (cfg.get_vars().find(aug_start_var) != cfg.get_vars().end())
        throw "augmented start variable connot be an existing variable";

    // check that the separator symbol is not a used variable/terminal
    const std::string separator("·");
    if (cfg.get_vars().find(separator) != cfg.get_vars().end() &&
        cfg.get_terms().find(separator.at(0)) != cfg.get_terms().end())
        throw "separator is not an existing terminal or variable";

    // keep track of all produced items
    // Note: this cannot be a set, because an ordering using indices needs to be kept.
    //       However, any `Item` in the vector should be unique.
    std::vector<ItemSet> itemsets{};
    std::map<unsigned int, std::map<std::string, unsigned int>> transitions{};

    // keep track of items that need to be processed
    std::queue<ItemSet> pending_items{};

    // add starting item to queue
    const ItemSet start_item =
        this->closure(ItemSet{{aug_start_var, {separator, cfg.get_start_var()}}}, separator, cfg);
    itemsets.push_back(start_item);
    pending_items.push(start_item);

    while (!pending_items.empty()) {
        const ItemSet& item_set = pending_items.front();

        const auto pending_item_set_it = std::find(itemsets.begin(), itemsets.end(), item_set);

        // DEBUG: `pending_item_set` should always be found
        assert((pending_item_set_it != itemsets.end() && "pending_item_set is found in itemsets"));

        const unsigned int pending_item_set_index = pending_item_set_it - itemsets.begin();

        // generate new items and transitions
        for (const Item& item : item_set) {
            // try to move the `separator` one right and generate new item_set

            // find separator in item
            auto sep_it = std::find(item.second.begin(), item.second.end(), separator);

            // DEBUG: check that the separator is found
            assert((sep_it != item.second.end() && "separator is found in item"));

            // check if the separator is at the end of the body
            if (sep_it + 1 == item.second.end())
                continue;

            const std::string transition_var = *(sep_it + 1);

            const Item new_item(item.first, [&]() -> CFG::Body {
                CFG::Body new_body(item.second);

                // delete old separator from body and advance iterator to location the separator needs to be
                // inserted
                auto new_sep_it = new_body.erase(new_body.begin() + (sep_it - item.second.begin()));
                new_body.insert(new_sep_it + 1, separator);

                return new_body;
            }());

            const ItemSet new_item_set = this->closure(ItemSet{{new_item}}, separator, cfg);

            // keep track of whether the item is found to not add it to the queue again
            bool found{true};
            // set index of new_item_set. If it is already present, find it. Else use new index
            auto new_item_set_it = std::find(itemsets.begin(), itemsets.end(), new_item_set);
            if (new_item_set_it == itemsets.end()) {
                // insert new_item_set and remember iterator
                new_item_set_it = itemsets.insert(itemsets.end(), new_item_set);
                found = false;
            }

            const unsigned int new_item_set_index = new_item_set_it - itemsets.begin();

            transitions[pending_item_set_index][transition_var] = new_item_set_index;

            if (!found)
                pending_items.push(new_item_set);
        }

        pending_items.pop();
    }
}

LR::ItemSet LR::closure(const ItemSet& item, const std::string& separator, const CFG& cfg) const {
    ItemSet result(item);

    bool changed{true};
    while (changed) {
        changed = false;

        for (const Item& item : result) {
            // find separator in item
            auto it = std::find(item.second.begin(), item.second.end(), separator);

            // DEBUG: check that the separator is found
            assert((it != item.second.end() && "separator is found in item"));

            // check that iterator is in the body and that the found value is not a terminal
            std::advance(it, 1);
            if (it == item.second.end() ||
                (it->length() == 1 && cfg.get_terms().find(it->at(0)) != cfg.get_terms().end()))
                continue;

            const std::set<CFG::Body>& rule = cfg.get_rules().at(*it);

            for (const CFG::Body& body : rule) {
                const Item new_item{*it, [&]() -> CFG::Body {
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
