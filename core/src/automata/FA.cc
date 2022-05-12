#include "FA.h"

FA::FA(const std::string &file) {
    // Load file
    std::ifstream i(file);
    nlohmann::json j;
    i >> j;

    // Parse alphabet
    for (const auto &c : j["alphabet"]) alphabet.push_back(std::string(c).front());

    // Parse states
    for (const auto &state : j["states"]) {
        State *stateParsed =
          new State{.name = state["name"], .starting = state["starting"], .accepting = state["accepting"]};

        states.push_back(stateParsed);
    }

    // Parse transitions
    for (const auto &transition : j["transitions"]) {
        State *from = findState(transition["from"]);
        State *to = findState(transition["to"]);

        Transition *transitionParsed =
          new Transition{.from = from, .to = to, .input = std::string(transition["input"]).front()};

        transitions.push_back(transitionParsed);
    }
}

FA::FA(const std::vector<char> alphabet, const std::vector<State *> states,
       const std::vector<Transition *> transitions) :
    alphabet(alphabet), states(states), transitions(transitions) {}

FA::~FA() {
    for (auto state : states) { delete state; }

    for (auto transition : transitions) { delete transition; }
}

void FA::print() const {
    // Initialize json object
    nlohmann::json j;

    // Add all properties
    j["type"] = "DFA";
    j["alphabet"] = alphabet;
    j["states"] = states;
    j["transitions"] = transitions;

    // Print
    std::cout << std::setw(4) << j << '\n';
}

void FA::print(std::ostream &out) const {
    // Initialize json object
    nlohmann::json j;

    std::vector<std::string> stringAlphabet;
    for (const char ch : alphabet) {
        stringAlphabet.push_back(std::string() + ch);
    }

    // Add all properties
    j["type"] = "DFA";
    j["alphabet"] = stringAlphabet;
    j["states"] = states;
    j["transitions"] = transitions;

    // Print
    out << std::setw(4) << j << '\n';
}

std::string FA::stateSetToName(const std::set<State *> &s) {
    // Void state edge case
    if (s.empty()) return "∅";

    // Convert to vector as we cannot
    // sort sets
    std::vector<State *> v(s.begin(), s.end());

    // Sort states alphabetically
    std::sort(v.begin(), v.end(), [](State *a, State *b) { return (a->name < b->name); });

    // Create the new state name
    std::string targetName = "{";
    for (auto c : v) targetName += c->name + ", ";
    targetName = targetName.substr(0, targetName.size() - 2) + "}";

    return targetName;
}

State *FA::findState(const std::string &n) const {
    for (State *state : states)
        if (state->name == n) return state;
    return nullptr;
}

std::vector<Transition *> FA::findTransition(const State *from, const char input) const {
    std::vector<Transition *> result;
    for (Transition *transition : transitions)
        if (transition->from == from && transition->input == input) result.push_back(transition);
    return result;
}

void FA::setStates(const std::vector<State *> &s) { states = s; }

void FA::setTransitions(const std::vector<Transition *> &t) { transitions = t; }

void FA::setAlphabet(const std::vector<char> &a) { alphabet = a; }

void FA::renameState(const std::string &s, const std::string &n) {
    for (auto &state : states) {
        if (state->name == s) state->name = n;
    }
}

void FA::removeUnused() {
    for (auto state = states.begin(); state != states.end(); ++state) {
        bool used = false;
        for (Transition *t : transitions) {
            if (t->from == *state || t->to == *state) {
                used = true;
                break;
            }
        }

        if (!used) {
            states.erase(state);
            state--;
        }
    }
}

State *FA::findStartingState() const {
    for (auto s : states)
        if (s->starting) return s;
    return nullptr;
}

bool FA::anyStarting(std::set<State *>& s) {
     for (auto c : s) if (c->starting) return c->starting;
    return false;
}

bool FA::anyStarting(std::vector<State *>& s) {
    for (auto c : s) if (c->starting) return c->starting;
    return false; 
}

bool FA::anyAccepting(std::vector<State *>& s) {
    for (auto c : s) if (c->accepting) return c->accepting;
    return false; 
}

bool FA::anyAccepting(std::set<State *>& s) {
    for (auto c : s) if (c->accepting) return c->accepting;
    return false; 
}