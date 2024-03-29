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
                // remove separator from item
                auto lookup_rule(item);
                auto lookup_sep_it = std::find(lookup_rule.second.begin(), lookup_rule.second.end(), separator);

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

LR::ParseResult LR::parse(StreamReader in) const {
    bool success{true};
    std::vector<ParseError> errors{};

    Lexer lexer(in);
    std::stack<StackContent> stack(std::deque<StackContent>{{std::nullopt, 0}});

    unsigned int parser_state = stack.top().second;
    Lexer::NextToken lexer_token{};
    do {
        lexer_token = lexer.get_next_token();

        // ignore some `TokenTypes`
        if (lexer_token.type == Lexer::TokenType::Comment || lexer_token.type == Lexer::TokenType::Whitespace)
            continue;

        const std::string lookup_token = this->lexer_token_to_parse_string(lexer_token);

        bool result{};
        try {

            // check what action to perform
            // `new_state` is both used as the new state and in case of a reduction, the rule to be used.
            const auto action_pair = this->table.at(parser_state).first.at(lookup_token);
            result = this->handle_action(action_pair, lookup_token, lexer_token, stack, parser_state);
        } catch (const std::out_of_range&) {
            success = false;
            auto best_corrections = this->handle_error(lookup_token, lexer, parser_state, stack);

            if (lexer_token.type != Lexer::TokenType::Eof) {
                std::vector<std::string> corrections{};

                for (const auto& correction : best_corrections) {
                    corrections.push_back(std::string() +
                                          ((correction.first == ErrorCorrectionAction::Delete)
                                               ? "delete the unexpected token"
                                               : std::string() + "insert token: `" + correction.second.value() +
                                                     "` before the unexpected token"));
                }

                errors.push_back({lexer_token.start.first,
                                  std::string() + "unexpected token found: `" + lexer_token.value + "`",
                                  std::move(corrections)});
            }

            // DEBUG: Because `Insert` corrections are added first to vector, first element can only be delete if
            // size is 0. DEBUG: At least one correction should be found
            assert(best_corrections.size() != 0 && "at least one correction is found");
            const auto& correction = best_corrections.at(0);

            switch (correction.first) {
            case ErrorCorrectionAction::Insert: {
                // try correction
                try {
                    const std::string& correction_token = correction.second.value();

                    // DEBUG: `correction_token` should be convertible to Lexer::TokenType
                    assert(Lexer::terminal_to_token_type.find(correction_token) !=
                               Lexer::terminal_to_token_type.end() &&
                           "correction token can be converted to Lexer::TokenType");

                    const auto action_pair = this->table.at(parser_state).first.at(correction.second.value());
                    this->handle_action(
                        action_pair, correction.second.value(),
                        Lexer::NextToken{
                            Lexer::terminal_to_token_type.at(correction_token), {0, 0}, {0, 0}, correction_token},
                        stack, parser_state);
                } catch (std::out_of_range&) {
                    throw std::runtime_error("solution to error does not fix the first iteration");
                }

                parser_state = stack.top().second;

                // try previous faulty lexer token
                try {
                    const auto action_pair = this->table.at(parser_state).first.at(lookup_token);
                    this->handle_action(action_pair, lookup_token, lexer_token, stack, parser_state);
                } catch (std::out_of_range&) {
                    throw std::runtime_error("solution to error does not fix the first iteration");
                }

                break;
            }
            case ErrorCorrectionAction::Delete:
                continue;
            }
        }

        if (!result)
            success = false;

        parser_state = stack.top().second;
    } while (lexer_token.type != Lexer::TokenType::Eof);

    std::optional<std::unique_ptr<ASTree>> tree{};
    if (stack.top().first.has_value() &&
        std::holds_alternative<std::tuple<CFG::Var, ASTree*>>(stack.top().first.value())) {
        tree = std::make_optional(
            std::unique_ptr<ASTree>(&*std::get<1>(std::get<std::tuple<CFG::Var, ASTree*>>(stack.top().first.value()))));
    } else {
        tree = std::nullopt;
    }

    ParseResult result{success, std::move(tree), std::move(errors)};

    return result;
}

bool LR::handle_action(ActionPair action_pair, const std::string& lookup_token, const Lexer::NextToken& lexer_token,
                       std::stack<StackContent>& stack, unsigned int& parser_state) const {
    bool success{true};

    switch (action_pair.first) {
    case LR::ActionType::Shift: {
        stack.emplace(lexer_token, action_pair.second);
        break;
    }
    case LR::ActionType::Reduce: {
        // reduce rule body amount of tokens
        const auto& rule = this->rules.at(action_pair.second);
        const unsigned int rule_length = rule.second.size();

        // TODO: build the parse tree
        std::vector<std::shared_ptr<ASTNode>> old_nodes;
        old_nodes.reserve(rule_length);
        for (unsigned int i = 0; i < rule_length; ++i) {
            const auto [pos_content, state] = stack.top();

            // DEBUG: `pos_content` should not be empty, this means that the starting stack state is popped
            assert(pos_content.has_value() && "pos_content is not empty");

            const auto content = pos_content.value();

            // check if content is a variable with tree or a terminal without tree
            if (std::holds_alternative<std::tuple<CFG::Var, ASTree*>>(content)) {
                const auto tree = std::get<1>(std::get<std::tuple<CFG::Var, ASTree*>>(content));
                old_nodes.push_back(tree->getRoot());
            } else {
                old_nodes.push_back(std::make_shared<ASTNode>(
                    new std::variant<CFG::Var, Lexer::NextToken>(std::get<Lexer::NextToken>(content)),
                    std::vector<std::shared_ptr<ASTNode>>{}));
            }

            stack.pop();
        }
        std::reverse(old_nodes.begin(), old_nodes.end());
        const std::string new_var = this->rules.at(action_pair.second).first;

        // TODO: split this up
        const StackContent new_pair{
            std::make_tuple(new_var,
                            new ASTree{new ASTNode(new std::variant<CFG::Var, Lexer::NextToken>(new_var), old_nodes)}),
            this->table.at(stack.top().second).second.at(this->rules.at(action_pair.second).first)};

        stack.push(new_pair);

        parser_state = stack.top().second;

        // check what the new action is
        const auto new_action_pair = this->table.at(parser_state).first.at(lookup_token);

        const auto result = this->handle_action(new_action_pair, lookup_token, lexer_token, stack, parser_state);

        if (!result)
            success = false;

        break;
    }
    case LR::ActionType::Accept: {
        break;
    }
    default:
        assert(false && "unreachable");
    }

    return success;
}

std::vector<std::pair<LR::ErrorCorrectionAction, std::optional<std::string>>> LR::handle_error(
    const std::string& current_lexer_token, const Lexer& lexer, unsigned int parser_state,
    std::stack<StackContent> stack) const {
    /*
        Try to insert every terminal that is expected in the current parser state and try to delete the current token.
        Keep on parsing until the next error.

        Select the best solution that fixes the input for the largest amount. If there is a tie, select the solution(s)
        that uses an insertion.
    */

    std::vector<std::tuple<ErrorCorrectionAction, std::optional<std::string>, unsigned int>>
        possible_correction_results{};

    for (unsigned int i = 0; i <= this->table.at(parser_state).first.size(); ++i) {
        bool token_delete{false};
        std::optional<std::string> fix_token{};

        std::stack<StackContent> tmp_stack(stack);
        unsigned int tmp_parser_state = tmp_stack.top().second;
        Lexer tmp_lexer(lexer);

        if (i == this->table.at(parser_state).first.size()) {
            token_delete = true;
        } else {
            try {
                fix_token = std::next(this->table.at(tmp_parser_state).first.begin(), i)->first;
                this->handle_action(
                    this->table.at(tmp_parser_state).first.at(fix_token.value()), fix_token.value(),
                    Lexer::NextToken{
                        Lexer::terminal_to_token_type.at(fix_token.value()), {0, 0}, {0, 0}, fix_token.value()},
                    tmp_stack, tmp_parser_state);
                tmp_parser_state = tmp_stack.top().second;
            } catch (std::out_of_range&) {
                // skip the current fix when it doesn't even fix anything (usually an unwanted reduction)
                continue;
            }
        }

        // deletion cannot parse the faulty token, thus give it a head start of 1
        unsigned int success_count = (token_delete) ? 1 : 0;

        Lexer::NextToken lexer_token_pair{};
        do {
            // determine what the `lexer_token` is
            std::string lexer_token{};
            if (!token_delete && success_count == 0) {
                // try previous faulty token first
                lexer_token = current_lexer_token;
            } else {
                lexer_token_pair = tmp_lexer.get_next_token();
                lexer_token = lexer_token_pair.value;

                // ignore some `TokenTypes`
                if (lexer_token_pair.type == Lexer::TokenType::Comment ||
                    lexer_token_pair.type == Lexer::TokenType::Whitespace)
                    continue;

                lexer_token = this->lexer_token_to_parse_string(lexer_token_pair);
            }

            try {
                const auto action_pair = this->table.at(tmp_parser_state).first.at(lexer_token);
                this->handle_action(
                    action_pair, lexer_token,
                    Lexer::NextToken{Lexer::terminal_to_token_type.at(lexer_token), {0, 0}, {0, 0}, lexer_token},
                    tmp_stack, tmp_parser_state);
            } catch (std::out_of_range&) {
                break;
            }

            tmp_parser_state = tmp_stack.top().second;
            ++success_count;
        } while (lexer_token_pair.type != Lexer::TokenType::Eof);

        possible_correction_results.emplace_back(
            (token_delete) ? ErrorCorrectionAction::Delete : ErrorCorrectionAction::Insert, fix_token, success_count);
    }

    // dbg(possible_correction_results);

    // Check what solution is best using the following conditions.
    // 1. Choose the solution that has the largest effect.
    // 2. Choose the solution that is an insertion.
    unsigned int max_effect{0};
    std::vector<decltype(possible_correction_results)::const_iterator> pos_solutions{};
    for (auto correction_result_it = possible_correction_results.begin();
         correction_result_it != possible_correction_results.end(); ++correction_result_it) {
        unsigned int effect = std::get<2>(*correction_result_it);
        if (max_effect > effect)
            continue;
        else if (effect != max_effect) {
            max_effect = effect;
            pos_solutions.clear();
        }

        pos_solutions.emplace_back(correction_result_it);
    }

    std::vector<std::pair<ErrorCorrectionAction, std::optional<std::string>>> solutions{};
    for (const auto& solution : pos_solutions) {
        solutions.emplace_back(std::get<0>(*solution), std::get<1>(*solution));
    }

    return solutions;
}
