#ifndef IDE_SRC_MACHINES_CFG_H
#define IDE_SRC_MACHINES_CFG_H

#include "nlohmann/json.hpp"

#include "../automata/Value.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

class CFG {
public:
    using Body = std::vector<std::string>;
    using Rule = std::pair<std::string, std::set<Body>>;
    using Rules = std::map<std::string, std::set<Body>>;
    using Var = std::string;
    using Term = char;
    using Vars = std::set<Var>;
    using Terms = std::set<Term>;

public:
    CFG() = default;
    CFG(const std::string& filepath);

    void print(std::ostream& out = std::cout) const;

    void toCNF(std::ostream& out = std::cout);

    void ll(std::ostream& out = std::cout) const;

    bool accepts(const std::string& input);

    inline void setStart(Value* s) { start_var = s->getName(); };

    void setVariables(Values V);

    void setTerminals(Values T);

    void setProductions(std::map<std::string, std::vector<std::vector<Value*>>> P);

    inline const Rules& get_rules() const { return this->rules; }
    inline const Vars& get_vars() const { return this->vars; }
    inline const Terms& get_terms() const { return this->terms; }
    inline const std::string& get_start_var() const { return this->start_var; }

private:
    void elim_eps_prods(std::ostream& out = std::cout);
    void elim_unit_pairs(std::ostream& out = std::cout);
    void elim_useless_symbs(std::ostream& out = std::cout);
    void replace_bad_bodies(std::ostream& out = std::cout);
    void split_bodies(std::ostream& out = std::cout);

    std::set<std::string> first(const std::vector<std::string>& b) const;
    std::set<std::string> follow(const std::string& a) const;
    bool has_eps_prod(const std::string& var) const;

    unsigned int rule_count() const;
    std::string find_var_with_prod(const Body& b) const;

    std::set<std::string> getLeft(const std::string& r);

    std::set<std::string> match(std::map<int, std::vector<std::set<std::string>>>& table, int col, int row, int i);
    void printTable(std::map<int, std::vector<std::set<std::string>>>& table);

    Body stringToBody(std::string str);

private:
    Vars vars;
    Terms terms;
    Rules rules;
    Var start_var;

#ifdef TEST
private:
    friend class CFGTest;
#endif
};

#endif // IDE_SRC_MACHINES_CFG_H
