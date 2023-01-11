#include "lr.h"

#include <algorithm>
#include <cassert>
#include <queue>

/// Local private utility namespace.
namespace {

std::vector<unsigned int> find_occurrences(const CFG::Body& b, const std::string& a) {
    std::vector<unsigned int> indices;
    auto it = b.begin();
    while ((it = std::find_if(it, b.end(), [&](const std::string& e) { return e == a; })) != b.end()) {
        indices.push_back(std::distance(b.begin(), it));
        it++;
    }
    return indices;
}

/// Calculates follow sets of grammar.
std::unordered_map<std::string, std::set<std::string>> follow(const CFG& cfg, const std::string& start_var,
                                                              const std::string& end_of_string) {
    std::unordered_map<std::string, std::set<std::string>> result{};

    for (const std::string& var : cfg.get_vars())
        result[var] = {};

    result[start_var].insert(end_of_string);
    // TODO: Nasty workaround, because cfg is not modified. It is assumed that there should be a production `start_var`
    // -> `cfg.get_start_var()`
    result[cfg.get_start_var()].insert(end_of_string);

    bool changed{true};
    while (changed) {
        changed = false;

        for (const auto& rule_set : cfg.get_rules()) {
            for (const auto& body : rule_set.second) {
                for (const std::string& var : cfg.get_vars()) {
                    const std::vector<unsigned int> occurrences = find_occurrences(body, var);

                    for (unsigned int oc : occurrences) {
                        const CFG::Body rem{body.begin() + oc + 1, body.end()};

                        if (!rem.empty()) {
                            // production if of form `αBβ` => FOLLOW(B) = FIRST(β) AND (if FIRST(β) contains ε)
                            // FOLLOW(B) = FOLLOW(A)
                            auto first_other = cfg.first(rem);

                            const auto eps_it = first_other.find("");
                            if (eps_it != first_other.end()) {
                                // remove ε from `first_other` and insert FOLLOW(A)
                                first_other.erase(eps_it);
                                const auto follow_a = result[rule_set.first];

                                first_other.insert(follow_a.begin(), follow_a.end());
                            }

                            const unsigned long length_before{result[var].size()};
                            result[var].insert(first_other.begin(), first_other.end());

                            if (result[var].size() > length_before)
                                changed = true;
                        } else {
                            // production if of form `αB` => FOLLOW(B) = FOLLOW(A)
                            const unsigned long length_before{result[var].size()};
                            result[var].insert(result[rule_set.first].begin(), result[rule_set.first].end());

                            if (result[var].size() > length_before)
                                changed = true;
                        }
                    }
                }
            }
        }
    }

    return result;
}

} // namespace

LR::LR(const CFG& cfg) {
    // check that the augmented start variable is not an existing variable
    const std::string aug_start_var("S'");
    if (cfg.get_vars().find(aug_start_var) != cfg.get_vars().end())
        throw LRCFGParsingException(LRCFGParsingException::AugStartVarInvalid);

    // check that the separator symbol is not a used variable/terminal
    const std::string separator("·");
    if (cfg.get_vars().find(separator) != cfg.get_vars().end() &&
        cfg.get_terms().find(separator) != cfg.get_terms().end())
        throw LRCFGParsingException(LRCFGParsingException::SeparatorSymbInvalid);

    // check that the end of input marker is not a used terminal
    if (cfg.get_terms().find(this->end_of_input) != cfg.get_terms().end())
        throw LRCFGParsingException(LRCFGParsingException::EndOfInputMarkertotInvalid);

    // keep track of all produced items
    // Note: The `ItemSet` constructed from the starting rule of the grammer should always be first in the verctor.
    // Note: This cannot be a set, because an ordering using indices needs to be kept.
    //       However, any `Item` in the vector should be unique.
    std::vector<ItemSet> item_sets{};
    std::map<unsigned int, std::map<std::string, unsigned int>> transitions{};

    // precalculate follow set
    std::unordered_map<std::string, std::set<std::string>> follow_sets{follow(cfg, aug_start_var, this->end_of_input)};

    // keep track of items that need to be processed
    std::queue<ItemSet> pending_items{};

    // add starting item to queue
    const ItemSet start_item = LR::closure(ItemSet{{aug_start_var, {separator, cfg.get_start_var()}}}, separator, cfg);
    item_sets.push_back(start_item);
    pending_items.push(start_item);

    while (!pending_items.empty()) {
        const ItemSet& item_set = pending_items.front();

        const auto pending_item_set_it = std::find(item_sets.begin(), item_sets.end(), item_set);

        // DEBUG: `pending_item_set_it` should always be found
        assert((pending_item_set_it != item_sets.end() && "pending_item_set_it is found in item_sets"));

        const unsigned int pending_item_set_index = pending_item_set_it - item_sets.begin();

        // build new itemset for every symbol that can be traversed
        for (const Item& search_item : item_set) {
            // get symbol that needs to be traversed
            auto search_item_sep_it = std::find(search_item.second.begin(), search_item.second.end(), separator);

            // DEBUG: check that the separator is found
            assert((search_item_sep_it != search_item.second.end() && "separator is found in search item"));

            const auto search_traverse_symbol_it = std::next(search_item_sep_it, 1);

            // check that the symbol is not the end of the rule
            if (search_traverse_symbol_it == search_item.second.end())
                continue;

            ItemSet new_item_set{};

            // ceck for every item in itemset if that symbol can be traversed
            for (const Item& item : item_set) {
                auto sep_it = std::find(item.second.begin(), item.second.end(), separator);

                // DEBUG: check that the separator is found
                assert((sep_it != item.second.end() && "separator is found in item"));

                auto traverse_symbol_it = std::next(sep_it, 1);

                if (traverse_symbol_it == item.second.end() || *traverse_symbol_it != *search_traverse_symbol_it)
                    continue;

                const Item new_item(item.first, [&]() -> CFG::Body {
                    CFG::Body new_body(item.second);

                    // delete old separator from body and advance iterator to location the separator needs to be
                    // inserted
                    auto new_sep_it = new_body.erase(new_body.begin() + (sep_it - item.second.begin()));
                    new_body.insert(new_sep_it + 1, separator);

                    return new_body;
                }());

                new_item_set.insert(new_item);
            }

            new_item_set = LR::closure(new_item_set, separator, cfg);

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

            transitions[pending_item_set_index][*search_traverse_symbol_it] = new_item_set_index;

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

                    this->table[i].first[item_set_transitions_entry.first] = {ActionType::Shift,
                                                                              item_set_transitions_entry.second};
                }
            }
        }

        // check if item set has shift-reduce or reduce-reduce conflict and resolve them using FIRST and FOLLOW sets
        for (const auto& item : item_sets.at(i)) {
            // if item is final (eg.A -> α ·) add reduce entry for every terminal in FOLLOW(A)

            auto sep_it = std::find(item.second.begin(), item.second.end(), separator);

            // DEBUG: separator should be found
            assert(sep_it != item.second.end() && "separator is found in item");

            if (std::next(sep_it, 1) != item.second.end())
                continue;

            // add reduce entry for every terminal in FOLLOW(A)
            if (item.first != aug_start_var) {
                // remove separator from item and find production rule in cfg productions
                auto lookup_rule(item);
                auto lookup_sep_it = std::find(lookup_rule.second.begin(), lookup_rule.second.end(), separator);

                // DEBUG: separator should be found
                assert((lookup_sep_it != lookup_rule.second.end() && "separator is found in item body"));

                lookup_rule.second.erase(lookup_sep_it);

                auto lookup_rule_it = std::find(this->rules.begin(), this->rules.end(), lookup_rule);

                // DEBUG: rule should be found
                assert((lookup_rule_it != this->rules.end() && "lookup rule is found in cfg rules"));

                const unsigned int lookup_rule_index = lookup_rule_it - this->rules.begin();

                for (const std::string& term : follow_sets.at(item.first)) {
                    this->table[i].first[term] = {ActionType::Reduce, lookup_rule_index};
                }
            } else {
                // add accept entry for ItemSet that contains final item that starts with the augmented star variable

                // DEBUG: the body should be the regular start variable and the separator
                assert((item_sets.at(i).begin()->second == CFG::Body{cfg.get_start_var(), separator} &&
                        "body of accepting item consists of the CFG start variable and the separator symbol"));

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
            if (it == item.second.end() || cfg.get_terms().find(*it) != cfg.get_terms().end())
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

std::pair<bool, LR::ASTree*> LR::parse(StreamReader in) const {
    auto* tree = new ASTree{};
    bool success{true};

    Lexer lexer(in);
    std::stack<StackContent> stack(std::deque<StackContent>{{std::nullopt, 0}});

    unsigned int parser_state = stack.top().second;
    Lexer::NextToken lexer_token_pair{};
    do {
        lexer_token_pair = lexer.get_next_token();

        // ignore some `TokenTypes`
        if (lexer_token_pair.first == Lexer::TokenType::Comment ||
            lexer_token_pair.first == Lexer::TokenType::Whitespace)
            continue;

        // determine what the `lexer_token` is
        std::string lexer_token = std::get<2>(lexer_token_pair.second);
        if (lexer_token_pair.first == Lexer::TokenType::Identifier ||
            lexer_token_pair.first == Lexer::TokenType::Literal || lexer_token_pair.first == Lexer::TokenType::Eof) {
            lexer_token = (lexer_token_pair.first == Lexer::TokenType::Identifier) ? "identifier"
                          : (lexer_token_pair.first == Lexer::TokenType::Literal)  ? "literal"
                                                                                   : this->end_of_input;
        }

        // check what action to perform
        // `new_state` is both used as the new state and in case of a reduction, the rule to be used.
        const auto action_pair = this->table.at(parser_state).first.at(lexer_token);

        const auto result = this->handle_action(action_pair, lexer_token, stack, parser_state);

        if (!result.first)
            success = false;

        parser_state = stack.top().second;
    } while (lexer_token_pair.first != Lexer::TokenType::Eof);

    return {success, tree};
}

std::pair<bool, LR::ASTree*> LR::handle_action(ActionPair action_pair, const std::string& token,
                                               std::stack<StackContent>& stack, unsigned int& parser_state,
                                               bool top) const {
    bool success{true};

    switch (action_pair.first) {
    case LR::ActionType::Shift: {
        // shift `{token, state}` onto stack
        stack.emplace(token, action_pair.second);
        // dbg(std::make_pair(token, action_pair.second));
        break;
    }
    case LR::ActionType::Reduce: {
        // reduce rule body amount of tokens
        const auto& rule = this->rules.at(action_pair.second);
        const unsigned int rule_length = rule.second.size();

        // TODO: build the parse tree
        for (unsigned int i = 0; i < rule_length; ++i) {
            stack.pop();
        }

        // TODO: split this up
        const StackContent new_pair{
            std::make_tuple(this->rules.at(action_pair.second).first),
            this->table.at(stack.top().second).second.at(this->rules.at(action_pair.second).first)};

        stack.push(new_pair);

        parser_state = stack.top().second;

        // check what the new action is
        const auto new_action_pair = this->table.at(parser_state).first.at(token);

        const auto result = this->handle_action(new_action_pair, token, stack, parser_state, false);

        if (!result.first)
            success = false;

        break;
    }
    case LR::ActionType::Accept: {
        break;
    }
    default:
        assert(false && "not implemented");
    }

    return {success, nullptr};
}
