#include "CFG.h"

#include <iomanip>
#include <string>

// Note: Ignore the printing code, this was solely used for the evaluation platform and (because it was already
//       implemented) for tests.

using Body = std::vector<std::string>;

// TODO: use template
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& b) {
    out << '{';
    out << std::accumulate(b.begin(), b.end(), std::string(),
                           [](const std::string& s, const std::string& a) { return s.empty() ? a : s + ", " + a; });
    out << '}';

    return out;
}

std::ostream& operator<<(std::ostream& out, const std::set<std::string>& b) {
    out << '{';
    out << std::accumulate(b.begin(), b.end(), std::string(),
                           [](const std::string& s, const std::string& a) { return s.empty() ? a : s + ", " + a; });
    out << '}';

    return out;
}

std::ostream& operator<<(std::ostream& out, const std::set<char>& b) {
    out << '{';
    out << std::accumulate(b.begin(), b.end(), std::string(),
                           [](const std::string& s, char a) { return s.empty() ? std::string(1, a) : s + ", " + a; });
    out << '}';

    return out;
}

std::string print_body(const Body& b) {
    std::string out;
    out += '`';
    out += std::accumulate(b.begin(), b.end(), std::string(),
                           [](const std::string& s, const std::string& a) { return s.empty() ? a : s + " " + a; });
    out += '`';

    return out;
}

std::string find_var_with_prod(const std::map<std::string, std::set<Body>>& rules, const Body& b) {
    for (const auto& rule_set : rules) {
        for (const auto& body : rule_set.second) {
            if (body == b)
                return rule_set.first;
        }
    }

    return "";
}

template <typename T>
inline std::vector<T> subvec(const std::vector<T>& v, unsigned int b, unsigned int e) {
    auto first = v.begin() + b;
    auto last = v.begin() + e + 1;
    std::vector<T> vector(first, last);
    return vector;
}

std::vector<unsigned int> find_occurrences(const Body& b, const std::string& a) {
    std::vector<unsigned int> indices;
    auto it = b.begin();
    while ((it = std::find_if(it, b.end(), [&](const std::string& e) { return e == a; })) != b.end()) {
        indices.push_back(std::distance(b.begin(), it));
        it++;
    }
    return indices;
}

CFG::CFG(const std::string& filepath) {
    using json = nlohmann::json;

    // read contents of file and parse as json
    std::ifstream file(filepath);
    json cfg = json::parse(file);
    file.close();

    // read out variables
    std::set<std::string> vars_ = cfg["Variables"];
    this->vars = vars_;

    // read out terminals
    std::set<char> terms_;
    for (std::string term : cfg["Terminals"])
        terms_.insert(term[0]);
    this->terms = terms_;

    // read out start variable
    this->start_var = cfg["Start"];

    // read out rules
    for (const auto& prod : cfg["Productions"]) {
        // std::cout << prod << '\n';
        const std::string head = prod["head"];
        const Body body = prod["body"];

        // add rule to map
        this->rules[head].insert(body);
    }
}

void CFG::print(std::ostream& out) const {
    // print variables
    out << "V = " << this->vars << '\n';

    // print terminals
    out << "T = " << this->terms << '\n';

    // print rules
    out << "P = {\n";
    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            out << "  " << rule_set.first << " -> " << print_body(body) << '\n';
        }
    }
    out << "}\n";

    // print start variable
    out << "S = " << this->start_var << '\n';
}

/// Returns `First-Set` for the given body of symbols.
///
/// Implemented using the explanation of rules presented in this
/// (article)[https://www.jambe.co.nz/UNI/FirstAndFollowSets].
std::set<std::string> CFG::first(const std::vector<std::string>& b) const {
    std::set<std::string> result;

    if (b.size() == 1) {
        const std::string& var = b[0];

        // 1. If X is a terminal then First(X) is just X!
        if (var.length() == 1 && this->terms.find(var[0]) != this->terms.end())
            return {var};

        // if `var` has no productions
        if (this->rules.find(var) == this->rules.end())
            return {};

        // 2. If there is a Production X → ε then add ε to first(X)
        if (this->has_eps_prod(var))
            result.insert("");

        // 3. If there is a Production X → Y1Y2..Yk then add first(Y1Y2..Yk) to first(X)
        for (const auto& prod : this->rules.at(var)) {
            if (prod.empty())
                continue;

            const auto prod_first = this->first(prod);
            result.insert(prod_first.begin(), prod_first.end());
        }
    } else {
        // 4. First(Y1Y2..Yk) is either

        auto first_y1 = this->first({b[0]});

        //      1. First(Y1) (if First(Y1) doesn't contain ε)
        if (first_y1.find("") == first_y1.end())
            return first_y1;

        //      2. OR (if First(Y1) does contain ε) then First (Y1Y2..Yk) is everything in First(Y1) <except for ε > as
        //      well as everything in First(Y2..Yk)
        first_y1.erase(first_y1.find(""));
        result.insert(first_y1.begin(), first_y1.end());
        const auto rem_first = this->first({b.begin() + 1, b.end()});
        result.insert(rem_first.begin(), rem_first.end());
    }

    return result;
}

std::set<std::string> CFG::follow(const std::string& a) const {
    std::set<std::string> result;

    if (a == this->start_var)
        result.insert("<EOS>");

    for (const auto& rule_set : this->rules) {
        for (const auto& b : rule_set.second) {
            const auto occurrences = find_occurrences(b, a);

            for (auto oc : occurrences) {
                const Body rem{b.begin() + oc + 1, b.end()};

                if (rem.size() > 0) {
                    // production if of form `αBβ` => FOLLOW(B) = FIRST(β) AND (if FIRST(β) contains ε) FOLLOW(B) =
                    // FOLLOW(A)
                    auto first_other = this->first(rem);

                    const auto eps_it = first_other.find("");
                    if (eps_it != first_other.end()) {
                        // remove ε from `first_other` and insert FOLLOW(A)
                        first_other.erase(eps_it);
                        const auto follow_a = this->follow(rule_set.first);
                    }

                    result.insert(first_other.begin(), first_other.end());
                } else {
                    // production if of form `αB` => FOLLOW(B) = FOLLOW(A)
                    const auto follow_other = this->follow(rule_set.first);
                    result.insert(follow_other.begin(), follow_other.end());
                }
            }
        }
    }

    return result;
}

// TODO: add caching for already calculated first and follow sets
void CFG::ll(std::ostream& out) const {
    out << ">>> Building LL(1) Table\n";
    out << " >> First:\n";

    // find first set for all variables
    std::map<std::string, std::set<std::string>> first;
    for (const auto& var : this->vars) {
        first[var] = this->first({var});

        // print first-set in the order required by the grading platform
        out << var << ": {";
        bool eps = false;
        for (auto set_it = first.at(var).begin(); set_it != first.at(var).end(); ++set_it) {
            if (*set_it == "") {
                eps = true;
                continue;
            }

            out << ((set_it == std::next(first.at(var).end(), -1) && eps == false) ? *set_it : *set_it + ", ");
        }

        out << "}\n";
    }

    out << " >> Follow:\n";

    // find follow set for all variables
    std::map<std::string, std::set<std::string>> follow;
    for (const auto& var : this->vars) {
        follow[var] = this->follow(var);

        // print follow-set in the order required by the grading platform
        out << var << ": {";
        bool eps = false;
        for (auto set_it = follow.at(var).begin(); set_it != follow.at(var).end(); ++set_it) {
            if (*set_it == "") {
                eps = true;
                continue;
            }

            out << ((set_it == std::next(follow.at(var).end(), -1) && eps == false) ? *set_it : *set_it + ", ");
        }

        out << "}\n";
    }

    // build the table
    std::map<std::string, std::map<std::string, std::string>> table;

    for (const auto& var : this->vars) {
        for (char c : this->terms) {
            if (first.at(var).find(std::string(1, c)) == first.at(var).end()) {
                if (follow.at(var).find(std::string(1, c)) == follow.at(var).end())
                    // the transition results in an error
                    table[var][std::string(1, c)] = "<ERR>";
                else
                    table[var][std::string(1, c)] = "";
            } else {
                // find the rule that starts with that symbol
                Body c_b;
                for (const auto& b : this->rules.at(var))
                    if (b.size() > 0 && b[0] == std::string(1, c))
                        c_b = b;

                table[var][std::string(1, c)] = print_body(c_b);
            }
        }

        // check for end of string
        if (follow.at(var).find("<EOS>") == follow.at(var).end())
            // the transition results in an error
            table[var]["<EOS>"] = "<ERR>";
        else {
            // check if `var` has production for ε
            if (this->rules.at(var).find({}) == this->rules.at(var).end())
                table[var]["<EOS>"] = "<ERR>";
            else
                table[var]["<EOS>"] = "";
        }
    }
}

void CFG::elim_eps_prods(std::ostream& out) {
    out << " >> Eliminating epsilon productions\n";

    std::set<std::string> nullables;
    std::map<std::string, bool> checked; // {var, nullable}
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& var : this->vars) {
            if (checked.find(var) != checked.end() && checked.at(var)) {
                // `var` is nullable, thus continue
                continue;
            }

            for (const auto& rule : this->rules[var]) {
                bool nullable = true;
                for (const auto& c : rule) {
                    if (std::find(this->vars.begin(), this->vars.end(), c) != this->vars.end()) {
                        // check if produced variable is nullable
                        if (!checked[c]) {
                            nullable = false;
                        }
                    } else {
                        // `c` is not a space nor a variable, thus a terminal
                        nullable = false;
                    }
                }

                if (nullable) {
                    nullables.insert(var);
                    checked[var] = true;
                    changed = true;

                    break;
                }
            }
        }
    }

    out << "  Nullables are " << nullables << '\n';

    std::map<std::string, std::set<Body>> new_rules;
    std::queue<std::pair<std::string, Body>> pending;
    unsigned int old_rule_count = this->rule_count();
    unsigned int new_rule_count = 0;

    // add all `rules` to `pending` and `new_rules`, except epsilon productions
    for (const auto& rule_pair : this->rules) {
        for (const auto& body : rule_pair.second) {
            if (body.empty())
                continue;

            pending.push({rule_pair.first, body});

            const bool inserted = new_rules[rule_pair.first].insert(body).second;
            if (inserted)
                ++new_rule_count;
        }
    }

    while (!pending.empty()) {
        const auto& rule_pair = pending.front();

        for (const auto& var : nullables) {
            for (unsigned int i = 0; i < rule_pair.second.size(); ++i) {
                const std::string& body = rule_pair.second[i];

                if (body == var) {
                    auto new_rule = rule_pair.second;

                    auto it = new_rule.begin();
                    std::advance(it, i);
                    new_rule.erase(it);

                    if (new_rule.empty())
                        continue;

                    const bool inserted = new_rules[rule_pair.first].insert(new_rule).second;
                    if (inserted)
                        ++new_rule_count;

                    pending.push({rule_pair.first, new_rule});
                }
            }
        }

        pending.pop();
    }

    this->rules = new_rules;

    out << "  Created " << new_rule_count << " productions, original had " << old_rule_count << "\n\n";
    this->print(out);
    out << '\n';
}

void CFG::elim_unit_pairs(std::ostream& out) {
    // 2. eliminate unit pairs
    out << " >> Eliminating unit pairs\n";

    unsigned int unit_production_count = 0;
    std::map<std::string, std::set<Body>> units;
    for (const auto& var : this->vars)
        units[var].insert({var});

    // add unit productions in grammar to set
    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            if (body.size() != 1 || std::find(this->vars.begin(), this->vars.end(), body[0]) == this->vars.end())
                continue;

            // rule is a single variable
            units[rule_set.first].insert(body);
            ++unit_production_count;
        }
    }

    // infer all other unit pairs
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& unit : units) {
            for (const auto& body : unit.second) {
                const std::string& var = body[0];
                for (const auto& new_var : units.at(var)) {
                    if (unit.second.find(new_var) != unit.second.end())
                        continue;
                    units[unit.first].insert(new_var);
                    changed = true;
                }
            }
        }
    }

    out << "  Found " << unit_production_count << " unit productions\n";

    out << "  Unit pairs: {";
    for (auto unit_set_it = units.begin(); unit_set_it != units.end(); ++unit_set_it) {
        for (auto unit_it = unit_set_it->second.begin(); unit_it != unit_set_it->second.end(); ++unit_it) {
            if (!(unit_set_it == units.begin() && unit_it == unit_set_it->second.begin()))
                out << ", ";
            out << "(" << unit_set_it->first << ", " << (*unit_it)[0] << ")";
        }
    }
    out << "}\n";

    unsigned int new_rule_count = 0;
    unsigned int old_rule_count = this->rule_count();

    std::map<std::string, std::set<Body>> new_rules;

    // remove unit productions
    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            if (body.size() == 1 && units.find(body[0]) != units.end())
                continue;
            new_rules[rule_set.first].insert(body);
            ++new_rule_count;
        }
    }

    for (const auto& unit_set : units) {
        for (const auto& body : unit_set.second) {
            if (new_rules.find(body[0]) == new_rules.end())
                continue;
            for (const auto& rule : new_rules.at(body[0])) {
                const auto& result = new_rules[unit_set.first].insert(rule).second;
                if (result)
                    ++new_rule_count;
            }
        }
    }

    this->rules = new_rules;

    out << "  Created " << new_rule_count << " new productions, original had " << old_rule_count << "\n\n";
    this->print(out);
    out << '\n';
}

void CFG::elim_useless_symbs(std::ostream& out) {
    out << " >> Eliminating useless symbols\n";

    unsigned int useless_variable_count = 0;
    unsigned int useless_terminal_count = 0;
    unsigned int removed_production_count = 0;

    // 3.1. determine generating symbols
    std::set<std::string> generating;
    for (const auto& term : this->terms)
        generating.insert(std::string(1, term));

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& rule_set : this->rules) {
            if (generating.find(rule_set.first) != generating.end())
                continue;

            for (const auto& body : rule_set.second) {
                bool is_generating = true;
                for (const auto& c : body) {
                    if (generating.find(c) == generating.end()) {
                        is_generating = false;
                        break;
                    }
                }

                if (is_generating) {
                    changed = true;
                    generating.insert(rule_set.first);
                    break;
                }
            }
        }
    }

    out << "  Generating symbols: " << generating << '\n';

    std::map<std::string, std::set<Body>> new_rules;

    // 3.2. delete non generating symbols
    for (const auto& rule_set : this->rules) {
        for (const auto& rule : rule_set.second) {
            bool needed = true;
            for (const auto& c : rule) {
                if (generating.find(c) == generating.end()) {
                    needed = false;
                    break;
                }
            }

            if (needed)
                new_rules[rule_set.first].insert(rule);
            else
                ++removed_production_count;
        }
    }

    // 3.3. determine reachable symbols
    std::set<std::string> reachable{this->start_var};

    changed = true;
    while (changed) {
        changed = false;

        for (const auto& rule_set : new_rules) {
            if (reachable.find(rule_set.first) == reachable.end())
                continue;

            for (const auto& rule : rule_set.second) {
                for (const auto& c : rule) {
                    bool result = reachable.insert(c).second;
                    if (result)
                        changed = true;
                }
            }
        }
    }

    out << "  Reachable symbols: " << reachable << '\n';

    // 3.4. delete non reachable symbols
    auto old_new_rules = new_rules;
    new_rules.clear();

    for (const auto& rule_set : old_new_rules) {
        if (reachable.find(rule_set.first) == reachable.end()) {
            removed_production_count += rule_set.second.size();

            continue;
        }

        new_rules[rule_set.first] = rule_set.second;
    }

    this->rules = new_rules;

    std::set<std::string> useful;
    for (const auto& s : generating) {
        if (reachable.find(s) != reachable.end())
            useful.insert(s);
    }

    std::set<std::string> new_vars;
    for (const auto& var : this->vars) {
        if (useful.find(var) != useful.end())
            new_vars.insert(var);
        else
            ++useless_variable_count;
    }

    std::set<char> new_terms;
    for (const auto& term : this->terms) {
        if (useful.find(std::string(1, term)) != useful.end())
            new_terms.insert(term);
        else
            ++useless_terminal_count;
    }

    this->vars = new_vars;
    this->terms = new_terms;

    out << "  Useful symbols: " << useful << '\n';

    out << "  Removed " << useless_variable_count << " variables, " << useless_terminal_count << " terminals and "
        << removed_production_count << " productions\n\n";

    this->print(out);
    out << '\n';
}

void CFG::replace_bad_bodies(std::ostream& out) {
    out << " >> Replacing terminals in bad bodies\n";

    unsigned int original_rule_count = this->rule_count();
    unsigned int added_var_count = 0;

    std::map<std::string, std::set<Body>> new_rules;
    std::set<std::string> new_vars;
    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            if (body.size() == 1) {
                new_rules[rule_set.first].insert(body);
                continue;
            }

            bool bad_body = false;
            for (const auto& c : body) {
                if (c.length() == 1 && std::find(this->terms.begin(), this->terms.end(), c[0]) != this->terms.end()) {
                    bad_body = true;
                    break;
                }
            }

            // replace terms with variables
            if (bad_body) {
                Body new_body;
                for (const auto& c : body) {
                    if (std::find(this->vars.begin(), this->vars.end(), c) != this->vars.end())
                        new_body.push_back(c);
                    else {
                        // find var that produces terminal directly
                        std::string var = this->find_var_with_prod({c});
                        if (var.empty()) {
                            var = std::string() + '_' + c;
                            bool result = new_vars.insert(var).second;
                            if (result) {
                                ++added_var_count;
                                new_rules.insert({var, {{c}}});
                            }
                        }

                        new_body.push_back(var);
                    }
                }
                new_rules[rule_set.first].insert(new_body);
            } else {
                new_rules[rule_set.first].insert(body);
            }
        }
    }

    this->rules = new_rules;
    this->vars.insert(new_vars.begin(), new_vars.end());

    const unsigned int added_rule_count = this->rule_count();

    out << "  Added " << added_var_count << " new variables: {"
        << std::accumulate(new_vars.begin(), new_vars.end(), std::string(),
                           [](const std::string& s, const std::string& a) { return s.empty() ? a : s + ", " + a; })
        << "}\n";
    out << "  Created " << added_rule_count << " new productions, original had " << original_rule_count << "\n\n";

    this->print(out);
    out << '\n';
}

void CFG::split_bodies(std::ostream& out) {
    std::map<std::string, std::set<Body>> new_rules;
    std::queue<std::pair<std::string, Body>> pending;
    std::map<std::string, unsigned int> replace_var_counter;

    unsigned int broke_body_count = 0;
    unsigned int new_var_count = 0;

    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            if (body.size() < 3)
                new_rules[rule_set.first].insert(body);
            else {
                const auto new_body = subvec(body, 1, body.size() - 1);

                unsigned int var_count = replace_var_counter[rule_set.first];
                if (var_count == 0) {
                    var_count = 2;
                    replace_var_counter[rule_set.first] = 3;
                } else
                    ++replace_var_counter[rule_set.first];

                std::string new_var = rule_set.first + '_' + std::to_string(var_count);
                ++new_var_count;
                this->vars.insert(new_var);

                if (new_body.size() < 3)
                    new_rules[new_var].insert(new_body);
                else
                    pending.push({new_var, new_body});

                Body old_body{body[0]};
                old_body.push_back(new_var);
                new_rules[rule_set.first].insert(old_body);

                ++broke_body_count;
            }
        }
    }

    while (!pending.empty()) {
        const auto& rule = pending.front();

        const auto new_body = subvec(rule.second, 1, rule.second.size() - 1);

        unsigned int var_count = replace_var_counter[rule.first];
        if (var_count == 0) {
            var_count = 2;
            replace_var_counter[rule.first] = 3;
        } else
            ++replace_var_counter[rule.first];

        std::string new_var = rule.first + '_' + std::to_string(var_count);
        ++new_var_count;
        this->vars.insert(new_var);

        if (new_body.size() < 3)
            new_rules[new_var].insert(new_body);
        else {
            pending.push({new_var, new_body});
            ++broke_body_count;
        }

        Body old_body{rule.second[0]};
        old_body.push_back(new_var);
        new_rules[rule.first].insert(old_body);

        pending.pop();
    }

    this->rules = new_rules;

    out << " >> Broke " << broke_body_count << " bodies, added " << new_var_count << " new variables\n";
}

void CFG::toCNF(std::ostream& out) {
    out << "Original CFG:\n\n";
    this->print(out);
    out << "\n-------------------------------------\n\n";

    // 1. eliminate epsilon productions
    this->elim_eps_prods(out);

    // 2. eliminate unit pairs
    this->elim_unit_pairs(out);

    // 3. eliminate useless symbols
    this->elim_useless_symbs(out);

    // 4. replace terminals in bad bodies
    this->replace_bad_bodies(out);

    // 5. split bodies
    this->split_bodies(out);

    out << ">>> Result CFG:\n\n";
    this->print(out);
}

unsigned int CFG::rule_count() const {
    unsigned int count = 0;
    for (const auto& rule_pair : this->rules)
        count += rule_pair.second.size();

    return count;
}

std::string CFG::find_var_with_prod(const Body& b) const {
    for (const auto& rule_set : this->rules) {
        for (const auto& body : rule_set.second) {
            if (body == b)
                return rule_set.first;
        }
    }

    return "";
}

bool CFG::has_eps_prod(const std::string& var) const {
    if (this->rules.find(var) == this->rules.end())
        return false;

    return (this->rules.at(var).find({}) != this->rules.at(var).end());
}

bool CFG::accepts(const std::string& input) {
    std::map<int, std::vector<std::set<std::string>>> table;
    for (unsigned long row = 0; row <= input.length() - 1; ++row) {
        if (row == 0) {
            for (char c : input) {
                std::set<std::string> symbols = getLeft(std::string(1, c));
                table[0].push_back(symbols);
            }
        } else {
            for (unsigned long col = 0; col < input.length() - row; ++col) {
                std::set<std::string> symbols;
                for (unsigned long i = 0; i < row; ++i) {
                    std::set<std::string> matches = match(table, col, row, i);
                    for (const std::string& match : matches) {
                        std::set<std::string> newSymbols = getLeft(match);
                        symbols.insert(newSymbols.begin(), newSymbols.end());
                    }
                }
                table[row].push_back(symbols);
            }
        }
    }
    if (table[input.length() - 1][0].find(start_var) != table[input.length() - 1][0].end()) {
        return true;
    }
    return false;
}

std::set<std::string> CFG::getLeft(const std::string& r) {
    std::set<std::string> result;
    for (const auto& prod : rules) {
        for (const auto& right : prod.second) {
            if (right == stringToBody(r)) {
                result.insert(prod.first);
            }
        }
    }
    return result;
};

void CFG::printTable(std::map<int, std::vector<std::set<std::string>>>& table) {
    for (auto it = table.rbegin(); it != table.rend(); ++it) {
        std::cout << "| ";
        for (std::set<std::string> symbolsSet : it->second) {
            std::cout << "{";
            for (auto sIt = symbolsSet.begin(); sIt != symbolsSet.end(); sIt++) {
                if (sIt != symbolsSet.begin())
                    std::cout << ", ";
                std::cout << *sIt;
            }
            std::cout << "} | ";
        }
        std::cout << std::endl;
    }
}

std::set<std::string> CFG::match(std::map<int, std::vector<std::set<std::string>>>& table, int col, int row, int i) {
    std::set<std::string> result;
    std::set<std::string> left = table[i][col];
    std::set<std::string> right = table[row - 1 - i][col + i + 1];
    for (std::string l : left) {
        for (std::string r : right) {
            result.insert(l + " " + r);
        }
    }
    return result;
}
Body CFG::stringToBody(std::string str) {
    Body body;
    for (char c : str) {
        if (c == ' ')
            continue;
        body.push_back(std::string(1, c));
    }
    return body;
}
void CFG::setVariables(Values V) {
    for (auto pair : V) {
        assert(!pair.second->getIsTerminal());
        vars.insert(pair.second->getName());
    }
}

void CFG::setTerminals(Values T) {
    for (auto pair : T) {
        assert(pair.second->getIsTerminal());
        if (pair.second->getName() == " ")
            continue;
        terms.insert(pair.second->getName()[0]);
    }
}

void CFG::setProductions(std::map<std::string, std::vector<std::vector<Value*>>> P) {
    std::map<std::string, std::set<Body>> newRules;
    for (auto pair : P) {
        std::set<Body> newBodySet;
        for (std::vector<Value*> values : pair.second) {
            Body newBody;
            for (Value* val : values) {
                newBody.push_back(val->getName());
            }
            newBodySet.insert(newBody);
        }
        newRules.insert({pair.first, newBodySet});
    }
    rules = newRules;
}
