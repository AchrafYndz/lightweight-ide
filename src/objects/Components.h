#ifndef IDE_COMPONENTS_H
#define IDE_COMPONENTS_H

#include "../../lib/id/Id.h"
#include "../../lib/nlohman-json/json.hpp"

#include <string>

struct State {
    std::string name;
    bool starting;
    bool accepting;

    ~State() = default;

    State *copy() const { return new State{.name = name, .starting = starting, .accepting = accepting}; }

    bool operator==(const State &rhs) const {
        return (name == rhs.name && starting == rhs.starting && accepting == rhs.accepting);
    }
};

inline void to_json(nlohmann::json &j, const State *p) {
    j = nlohmann::json{{"name", p->name}, {"starting", p->starting}, {"accepting", p->accepting}};
}

struct Transition {
    const State *from;
    const State *to;
    const char input;

    ~Transition() = default;

    Transition *copy(State *_from, State *_to) const { return new Transition{.from = _from, .to = _to, .input = input}; }

    bool operator==(const Transition &rhs) const { return (from == rhs.from && to == rhs.to && input == rhs.input); }
};

inline void to_json(nlohmann::json &j, const Transition *p) {
    j = nlohmann::json{{"from", p->from->name}, {"to", p->to->name}, {"input", p->input}};
}

#endif  // IDE_COMPONENTS_H