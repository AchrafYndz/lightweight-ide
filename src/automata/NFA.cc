#include "NFA.h"

NFA::NFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
         const std::vector<Transition *> &transitions) :
    FA(alphabet, states, transitions) {}

DFA NFA::toDFA() const {
    // Fetch starting state
    State *start;
    for (auto state : states)
        if (state->starting) start = state;

    // Initialize new DFA properties
    std::map<std::string, State *> DFAStates;
    std::vector<Transition *> DFATransitions = {};

    // Initialize the algorithm
    std::set<State *> init = {start};
    DFAStates.insert(
      {stateSetToName(init), new State{.name = stateSetToName(init), .starting = true, .accepting = start->accepting}});

    std::vector<std::set<State *>> queue = {{start}};

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
                        targets.insert(transition->to->copy());
                    }
                }
            }

            // We now have all info for the specified letter
            // Let's create the new properties
            std::string targetName = stateSetToName(targets);

            // Check if accepted
            bool accepting = false;
            for (auto t : targets)
                if (t->accepting) accepting = true;

            // Create new state if necessary
            State *state = nullptr;
            if (DFAStates.end() == DFAStates.find(targetName)) {
                // Create new state as it has not been done
                // just yet
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