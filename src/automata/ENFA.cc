#include "ENFA.h"

ENFA::ENFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
           const std::vector<Transition *> &transitions, const char eps) :
    FA(alphabet, states, transitions), eps(eps) {}

ENFA::ENFA(const std::string &file) {
    // parse the file
    std::ifstream f(file);
    const nlohmann::json j = nlohmann::json::parse(f);

    // check the automata type
    assert(j["type"] == "ENFA");

    // parse alphabet
    for (const auto &ch : j["alphabet"]) alphabet.push_back(std::string(ch).front());
    eps = std::string(j["epsilon"]).front();

    // parse states
    for (const auto &state : j["states"]) {
        State *stateParsed =
          new State{.name = state["name"], .starting = state["starting"], .accepting = state["accepting"]};

        states.push_back(stateParsed);
    }

    // parse transitions
    for (const auto &transition : j["transitions"]) {
        State *from = findState(transition["from"]);
        State *to = findState(transition["to"]);

        Transition *transitionParsed =
          new Transition{.from = from, .to = to, .input = std::string(transition["input"]).front()};

        transitions.push_back(transitionParsed);
    }
}

bool ENFA::accepts(const std::string &str) const {
    State *startState = findStartingState();
    const std::vector<State *> tmp = closure(startState);
    std::set<State *> currentStates = std::set<State *>(tmp.begin(), tmp.end());

    for (const char ch : str) {
        std::set<State *> newCurrentStates;
        for (const auto &state : currentStates) {
            // check if a transition happens on the given character
            const std::vector<Transition *> tmp = findTransition(state, ch);
            const std::set<Transition *> transitions = std::set<Transition *>(tmp.begin(), tmp.end());

            if (transitions.empty()) continue;

            for (const auto &transition : transitions) {
                const std::vector<State *> closureStates = closure(transition->to);

                for (const auto newState : closureStates) newCurrentStates.insert(newState);
            }
        }

        currentStates = newCurrentStates;
    }

    // check if there is a finalState among currentStates
    for (const auto &state : currentStates) {
        if (state->accepting) return true;
    }

    return false;
}

std::vector<State *> ENFA::closure(State *state) const {
    std::vector<State *> currentStates = {state};
    std::set<State *> result = {state};

    while (!currentStates.empty()) {
        std::vector<State *> newCurrentStates;
        for (const auto &currentState : currentStates) {
            const std::vector<Transition *> transitions = findTransition(currentState, eps);
            if (!transitions.empty()) {
                for (const auto &transition : transitions) {
                    result.insert(transition->to);
                    newCurrentStates.push_back(transition->to);
                }
            }
        }
        currentStates = newCurrentStates;
    }

    return std::vector<State *>(result.begin(), result.end());
}

void ENFA::printStats(std::ostream &out) const {
    out << "no_of_states=" << states.size() << '\n';

    std::map<char, unsigned int> transitionCount;
    for (const auto &transition : transitions) { ++transitionCount[transition->input]; }

    // print amount of eps transitions
    out << "no_of_transitions[" << eps << "]=" << transitionCount[eps] << '\n';

    // print other char transitions
    for (const char ch : alphabet) out << "no_of_transitions[" << ch << "]=" << transitionCount[ch] << '\n';

    // collect degree information
    std::map<int, int> degreeCounter;
    for (const auto &state : states) {
        unsigned int count = 0;
        for (const auto &transition : transitions)
            if (transition->from == state) ++count;

        ++degreeCounter[count];
    }

    for (const auto &degreeCount : degreeCounter)
        out << "degree[" << degreeCount.first << "]=" << degreeCount.second << '\n';
}

void ENFA::printStats() const { printStats(std::cout); }