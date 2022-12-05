#include "lr.h"

#include <cassert>
#include <queue>

LR::LR(const CFG& cfg) {
    // check that the augmented start variable is not an existing variable
    const std::string aug_start_var("S'");
    if (cfg.get_vars().find(aug_start_var) != cfg.get_vars().end())
        throw LRCFGParsingException(LRCFGParsingException::AugStartVarInvalid);

    // check that the separator symbol is not a used variable/terminal
    const std::string separator("·");
    if (cfg.get_vars().find(separator) != cfg.get_vars().end() &&
        cfg.get_terms().find(separator.at(0)) != cfg.get_terms().end())
        throw LRCFGParsingException(LRCFGParsingException::SeparatorSymbInvalid);

    // check that the end of input marker is not a used terminal
    if (cfg.get_terms().find(this->end_of_input) != cfg.get_terms().end())
        throw LRCFGParsingException(LRCFGParsingException::EndOfInputMarkertotInvalid);

    // keep track of all produced items
    // Note: This cannot be a set, because an ordering using indices needs to be kept.
    //       However, any `Item` in the vector should be unique.
    // TODO: Refactor this to use pointers, then it _can_ be a set.
    std::vector<ItemSet> item_sets{};
    std::map<unsigned int, std::map<std::string, unsigned int>> transitions{};

    // keep track of items that need to be processed
    // TODO: using pointers, `ItemSet` does not need to be copied
    std::queue<ItemSet> pending_items{};

    // add starting item to queue
    const ItemSet start_item =
        this->closure(ItemSet{{aug_start_var, {separator, cfg.get_start_var()}}}, separator, cfg);
    item_sets.push_back(start_item);
    pending_items.push(start_item);

    while (!pending_items.empty()) {
        const ItemSet& item_set = pending_items.front();

        const auto pending_item_set_it = std::find(item_sets.begin(), item_sets.end(), item_set);

        // DEBUG: `pending_item_set_it` should always be found
        assert((pending_item_set_it != item_sets.end() && "pending_item_set_it is found in item_sets"));

        const unsigned int pending_item_set_index = pending_item_set_it - item_sets.begin();

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

            // keep track of whether the item is found, so it does not get added to the queue again
            bool found{true};
            // set index of new_item_set. If it is already present, find it. Else use new index
            auto new_item_set_it = std::find(item_sets.begin(), item_sets.end(), new_item_set);
            if (new_item_set_it == item_sets.end()) {
                // insert new_item_set and remember iterator
                new_item_set_it = item_sets.insert(item_sets.end(), new_item_set);
                found = false;
            }

            const unsigned int new_item_set_index = new_item_set_it - item_sets.begin();

            transitions[pending_item_set_index][transition_var] = new_item_set_index;

            if (!found)
                pending_items.push(new_item_set);
        }

        pending_items.pop();
    }

    // construct reduction rules
    for (CFG::Rule rule_set : cfg.get_rules())
        for (const CFG::Body& body : rule_set.second)
            this->rules.push_back({rule_set.first, body});

    // construct parsing table
    for (unsigned int i{0}; i < item_sets.size(); ++i) {

        // check if the `item_set` has any outgoing transitions
        auto item_set_transitions_it = transitions.find(i);
        if (item_set_transitions_it != transitions.end()) {
            const std::map<std::string, unsigned int>& item_set_transitions = item_set_transitions_it->second;

            for (const auto& item_set_transitions_entry : item_set_transitions) {
                // check if the transition happens on a variable, if so it is a *goto*, else it is an *action*
                if (cfg.get_vars().find(item_set_transitions_entry.first) != cfg.get_vars().end()) {
                    // *GOTO*
                    this->table[i].second[item_set_transitions_entry.first] = item_set_transitions_entry.second;
                } else {
                    // *ACTION*

                    // DEBUG: assert that the length is 1
                    assert(
                        (item_set_transitions_entry.first.length() == 1 && "item_set_transitions_entry length is 1"));

                    this->table[i].first[item_set_transitions_entry.first.at(0)] = {ActionType::Shift,
                                                                                    item_set_transitions_entry.second};
                }
            }
        }

        // if item_set is final (one entry and only one separator on the right), reduce its action table
        if (std::find(item_sets.at(i).begin()->second.begin(), item_sets.at(i).begin()->second.end(), separator) + 1 ==
            item_sets.at(i).begin()->second.end()) {

            // DEBUG: there should only be one item in the set
            assert((item_sets.at(i).size() == 1 && "only one item present in a final set"));

            if (item_sets.at(i).begin()->first != aug_start_var) {
                // remove separator from item and find production rule in cfg productions
                auto lookup_rule(*item_sets.at(i).begin());
                auto sep_it = std::find(lookup_rule.second.begin(), lookup_rule.second.end(), separator);

                // DEBUG: separator should be found
                assert((sep_it != lookup_rule.second.end() && "separator is found in item body"));

                lookup_rule.second.erase(sep_it);

                auto lookup_rule_it = std::find(this->rules.begin(), this->rules.end(), lookup_rule);

                // DEBUG: rule should be found
                assert((lookup_rule_it != this->rules.end() && "lookup rule is found in cfg rules"));

                const unsigned int lookup_rule_index = lookup_rule_it - this->rules.begin();

                // add a reduce entry for every terminal and end_of_string marker
                for (char term : cfg.get_terms()) {
                    this->table[i].first[term] = {ActionType::Reduce, lookup_rule_index};
                }
                this->table[i].first[end_of_input] = {ActionType::Reduce, lookup_rule_index};
            } else {
                // add accept entry for ItemSet that contains final item that starts with the augmented start variable

                // DEBUG: the body should be the regular start variable and the separator
                assert((item_sets.at(i).begin()->second == CFG::Body{cfg.get_start_var(), separator}));

                // add accept entry for end of string terminal
                this->table[i].first[this->end_of_input] = {ActionType::Accept, 0};
            }
        }
    }
}

// TODO: add caching
LR::ItemSet LR::closure(const ItemSet& item, const std::string& separator, const CFG& cfg) {
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
