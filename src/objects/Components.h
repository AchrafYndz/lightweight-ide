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
};

inline void to_json(nlohmann::json &j, const State *p) {
    j = nlohmann::json{{"name", p->name}, {"starting", p->starting}, {"accepting", p->accepting}};
}

struct Transition {
    const State *from;
    const State *to;
    const char input;

    ~Transition() = default;
};

inline void to_json(nlohmann::json &j, const Transition *p) {
    j = nlohmann::json{{"from", p->from->name}, {"to", p->to->name}, {"input", p->input}};
}

#endif  // IDE_COMPONENTS_H