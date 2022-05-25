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
            for (const auto &transition : transitions) {
                if (transition->to->name == state->name) continue;
                result.insert(transition->to);
                newCurrentStates.push_back(transition->to);
            }
        }
        currentStates = newCurrentStates;
    }
    return {result.begin(), result.end()};
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

DFA ENFA::toDFA() const {
    // Fetch starting state;
    State *start;
    start = findStartingState();

    // Get ε-Closure for start state
    std::vector<State *> startClosure = closure(start);

    // Initialize new DFA properties
    std::map<std::string, State *> DFAStates;
    std::vector<Transition *> DFATransitions = {};

    // Check if accepted
    bool accepting = false;
    for (auto s : startClosure)
        if (s->accepting) accepting = true;

    // Initialize the algorithm
    std::set<State *> init(startClosure.begin(), startClosure.end());
    DFAStates.insert(
      {stateSetToName(init), new State{.name = stateSetToName(init), .starting = true, .accepting = accepting}});

    std::vector<std::set<State *>> queue = {init};

    // Continue the algorithm while we have items left in the queue
    while (!queue.empty()) {
        // Starting points
        std::set<State *> starters = queue[0];
        std::string starterName = stateSetToName(starters);

        // Go over every character in the alphabet
        for (const char letter : alphabet) {
            // Determine all targets
            std::set<State *> targets;

            // Loop over all states we have to check
            for (auto from : starters) {
                // Check for transitions with this input
                for (auto transition : transitions) {
                    if (transition->from == from && transition->input == letter) {
                        // Get the ε-closure of this target
                        std::vector<State *> targetClosures = closure(transition->to);
                        // Add all the ε-closure's states to the targets
                        for (State *targetClosure : targetClosures) { targets.insert(targetClosure); }
                    }
                }
            }

            // We now have all the info for the specified letter
            // Let's create the new properties
            std::string targetName = stateSetToName(targets);

            // Check if accepted
            bool accepting = false;
            for (auto t : targets)
                if (t->accepting) accepting = true;

            // Create new state if necessary
            State *state = nullptr;
            if (DFAStates.end() == DFAStates.find(targetName)) {
                // Create new state as it has not been done just yet
                state = new State{.name = targetName, .starting = false, .accepting = accepting};
                DFAStates.insert({targetName, state});

                // Queue the new combined state for analysis if required
                // If we require a void state, create all the required transitions
                // for it to function accordingly
                if (targetName == "∅")
                    for (const char letter : alphabet)
                        DFATransitions.push_back(new Transition{.from = state, .to = state, .input = letter});
                else
                    queue.push_back(targets);
            } else
                state = DFAStates.at(targetName);

            // Create computed transition
            State *from = DFAStates.at(starterName);
            Transition *transition = new Transition{.from = from, .to = state, .input = letter};
            DFATransitions.push_back(transition);
        }
        // Erase the pair we just handled
        queue.erase(queue.begin());
    }

    // Create states vector and alphabet
    std::set<char> DFAAlphabetSet = {};
    std::vector<State *> DFAStatesVector = {};
    DFAStatesVector.reserve(DFAStates.size());
    for (auto &state : DFAStates) DFAStatesVector.push_back(state.second);
    for (auto &transition : DFATransitions) DFAAlphabetSet.insert(transition->input);

    // Convert alphabet to vector
    std::vector<char> DFAAlphabetVector(DFAAlphabetSet.begin(), DFAAlphabetSet.end());

    // Create DFA
    DFA dfa = DFA();
    dfa.setTransitions(DFATransitions);
    dfa.setStates(DFAStatesVector);
    dfa.setAlphabet(DFAAlphabetVector);

    // All done!
    return dfa;
}
