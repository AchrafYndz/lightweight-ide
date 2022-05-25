#include "DFA.h"

DFA::DFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
         const std::vector<Transition *> &transitions) :
    FA(alphabet, states, transitions) {}

DFA::DFA(const DFA &dfa1, const DFA &dfa2, bool cross) {
    // check that the alphabets are equal
    assert(dfa1.getAlphabet() == dfa2.getAlphabet());

    alphabet = dfa1.getAlphabet();

    // add the startState
    State *startSate =
      new State{.name = std::string("(") + dfa1.findStartingState()->name + ',' + dfa2.findStartingState()->name + ')',
                .starting = true,
                .accepting = false};
    states.push_back(startSate);

    // keep track of the composition of states
    std::map<std::string, std::pair<std::string, std::string>> stateComp;
    stateComp[startSate->name] = {dfa1.findStartingState()->name, dfa2.findStartingState()->name};

    // store the current states for lazy evaluation
    std::vector<State *> currentStates = {startSate};

    bool changed = true;
    while (changed) {
        changed = false;

        std::vector<State *> newStates;
        for (State *state : currentStates) {
            for (const char ch : alphabet) {
                State *dfa1State = dfa1.findTransition(dfa1.findState(stateComp[state->name].first), ch).front()->to;
                State *dfa2State = dfa2.findTransition(dfa2.findState(stateComp[state->name].second), ch).front()->to;

                const std::string newStateName = std::string("(") + dfa1State->name + ',' + dfa2State->name + ')';

                // check if a transition from state 'state' to state 'newState' is already present on character 'ch'
                if (findState(newStateName)) continue;

                changed = true;

                // update the state composition map
                stateComp[newStateName] = {dfa1State->name, dfa2State->name};

                // check if the newState needs to be accepting
                bool newStateAccepting = false;
                if (cross && (dfa1State->accepting && dfa2State->accepting)) {
                    newStateAccepting = true;
                } else if (!cross && (dfa1State->accepting || dfa2State->accepting)) {
                    newStateAccepting = true;
                }

                // create new state and transition
                State *newState = new State{.name = newStateName, .starting = false, .accepting = newStateAccepting};
                Transition *trans = new Transition{.from = state, .to = newState, .input = ch};

                states.push_back(newState);
                transitions.push_back(trans);

                newStates.push_back(newState);
            }
        }
        currentStates = newStates;
    }
}

DFA::DFA(std::vector<DFA> dfas, bool cross) {
    DFA currentDFA = dfas[0];
    for (const DFA &dfa : dfas) {
        if (currentDFA == dfas[0]) continue;
        currentDFA = DFA(currentDFA, dfa, cross);
    }
    alphabet = currentDFA.alphabet;
    states = currentDFA.states;
    transitions = currentDFA.transitions;
}

bool DFA::pairCrossed(TFA &t, const std::string &s1, const std::string &s2) {
    for (auto &row : t) {
        std::string pName = row.first;
        for (auto &col : row.second) {
            std::string qName = col.first;
            // These should contain the same value, so we can return the first
            // one we encounter.
            if ((pName == s1 && qName == s2) || (pName == s2 && qName == s1)) return col.second;
        }
    }

    return false;
}

std::set<std::set<State *>> DFA::findEquivalenceClasses(TFA &t) const {
    std::set<std::set<State *>> e = {};
    for (auto &state : states) {
        std::set<State *> result = findEquivalentStates(t, state);
        if (result.size() > 1) e.insert(result);
    }

    return e;
}

std::set<State *> DFA::findEquivalentStates(TFA &t, State *s) const {
    std::set<State *> e = {s};

    for (auto &row : t) {
        State *p = findState(row.first);
        for (auto &col : row.second) {
            if (col.second) continue;
            State *q = findState(col.first);

            if (e.find(p) != e.end() || e.find(q) != e.end()) {
                e.insert(p);
                e.insert(q);
            }
        }
    }

    return e;
}

TFA DFA::generateTFA() {
    // Create data structure to store TFA result
    // This data structure is ROW MAJOR
    TFA t;

    // Sort states
    sort(states.begin(), states.end(), [](State *a, State *b) { return (a->name < b->name); });

    // Initialize TFA structure
    for (auto state1 = (states.begin() + 1); state1 != states.end(); ++state1) {
        // Create row
        std::vector<std::pair<std::string, bool>> row;
        for (auto state2 = states.begin(); state2 != (states.end() - 1); ++state2)
            row.emplace_back((*state2)->name, false);

        // Insert row
        t.emplace_back((*state1)->name, row);
    }

    // First run
    for (auto &row : t) {
        // Fetch state p
        std::string pName = row.first;
        const State *p = findState(pName);

        // Loop over all columns
        for (auto &col : row.second) {
            // Fetch state q
            std::string qName = col.first;
            State *q = findState(qName);

            // If they're distinguishable, cross them out
            if (p->accepting != q->accepting) col.second = true;
        }
    }

    // All other runs
    // Loop while we're still adding
    // new crosses
    bool crossed = true;
    while (crossed) {
        // Reset crossed indicator
        // We'll be setting it later
        crossed = false;

        // Loop over all rows
        for (auto &row : t) {
            // Fetch state p
            std::string pName = row.first;
            const State *p = findState(pName);

            // Loop over all columns
            for (auto &col : row.second) {
                // Fetch state q
                std::string qName = col.first;
                const State *q = findState(qName);

                // If already marked as distinguishable, continue
                if (col.second) continue;

                // Check if the state is distinguishable
                // Try all possible transitions, if the results are distinguishable
                // then so are the starting states
                for (const char letter : alphabet) {
                    // Get transitions
                    Transition *transition1 = findTransition(p, letter).front();
                    Transition *transition2 = findTransition(q, letter).front();

                    // Check if distinguishable
                    if ((transition1 == nullptr && transition2 == nullptr) ||
                        ((transition1 != nullptr && transition2 != nullptr) &&
                         pairCrossed(t, transition1->to->name, transition2->to->name))) {
                        // Pair distinguishable
                        col.second = true;
                        crossed = true;
                        break;
                    }
                }
            }
        }
    }

    return t;
}

void DFA::printTable() {
    TFA t = generateTFA();
    printTable(t);
}

void DFA::printTable(TFA &t) {
    // Print
    for (auto row = t.begin(); row != t.end(); ++row) {
        // Fetch state and print name
        std::string pName = row->first;
        std::cout << pName;

        // Get row index
        int rowIndex = row - t.begin();

        // Loop over all columns
        for (auto col = row->second.begin(); col != row->second.end(); ++col) {
            // Get the state name
            std::string qName = col->first;

            // Print the boolean only if value is below or on the diagonal
            int colIndex = col - row->second.begin();
            if (colIndex <= rowIndex) std::cout << "	" << (col->second ? "X" : "-");
        }
        std::cout << '\n';

        // Print bottom names
        if (row == (t.end() - 1)) {
            std::cout << "	";
            for (auto &col : row->second) { std::cout << col.first << "	"; }
            std::cout << '\n';
        }
    }
}

DFA DFA::minimize() {
    // Perform table filling algorithm
    TFA t = generateTFA();

    // Create equivalence classes
    // Create the minimized DFA
    std::vector<Transition *> minTransitions = {};
    std::vector<State *> minStates = {};

    // Create new states and transitions
    // Create DFA, we'll use this to access findState
    DFA dfa = DFA();
    // Recreate all states and assign them to the DFA
    for (auto state : states) minStates.push_back(state->copy());
    dfa.setStates(minStates);
    // Recreate all transitions with the new states
    for (auto transition : transitions)
        minTransitions.push_back(
          transition->copy(dfa.findState(transition->from->name), dfa.findState(transition->to->name)));

    // Get all equivalence classes
    std::set<std::set<State *>> classes = findEquivalenceClasses(t);
    // Recreate equivalence classes as we don't want shared
    // pointers
    std::set<std::set<State *>> newClasses = {};
    for (const auto &c : classes) {
        std::set<State *> r = {};
        for (auto s : c) r.insert(dfa.findState(s->name));
        newClasses.insert(r);
    }

    // Condense states and transitions
    for (auto c : newClasses) {
        // Create the new state
        std::string name = stateSetToName(c);
        bool starting = anyStarting(c);
        bool accepting = anyAccepting(c);

        State *combined = new State{.name = name, .starting = starting, .accepting = accepting};
        minStates.push_back(combined);

        // Re-route all transitions
        for (auto s : c) {
            for (auto transition : minTransitions) {
                if (transition->from == s) transition->from = combined;
                if (transition->to == s) transition->to = combined;
            }
        }
    }

    // Filter duplicates manually
    std::vector<Transition *> seen = {};
    for (auto transition = minTransitions.begin(); transition != minTransitions.end(); ++transition) {
        auto is_equal = [&transition](Transition *t) { return (*t == **transition); };
        if (std::find_if(seen.begin(), seen.end(), is_equal) != seen.end()) {
            minTransitions.erase(transition);
            transition--;
            continue;
        }

        seen.push_back(*transition);
    }

    // Update DFA
    dfa.setTransitions(minTransitions);
    dfa.setStates(minStates);
    dfa.setAlphabet(alphabet);
    dfa.removeUnused();

    // Rename all states to have brackets
    for (auto state : minStates)
        if (state->name[0] != '{') dfa.renameState(state->name, "{" + state->name + "}");

    // Breakpoint temp
    return dfa;
}

bool DFA::operator==(DFA &rhs) {
    // If the alphabet does not match, the DFAs certainly aren't
    // equivalent as there should be no excess characters in the alphabet

    // Let's start by sorting the alphabet to make sure there's
    // no difference in order
    sort(rhs.alphabet.begin(), rhs.alphabet.end());
    sort(alphabet.begin(), alphabet.end());
    // Check for equivalence
    if (rhs.alphabet != alphabet) return false;

    // Combine and sort states
    std::vector<State *> joinedStates = states;
    joinedStates.insert(joinedStates.end(), rhs.states.begin(), rhs.states.end());
    sort(joinedStates.begin(), joinedStates.end(), [](State *a, State *b) { return (a->name < b->name); });

    // Combine and sort transitions
    std::vector<Transition *> joinedTransitions = transitions;
    joinedTransitions.insert(joinedTransitions.end(), rhs.transitions.begin(), rhs.transitions.end());

    DFA dfa = DFA();
    dfa.setTransitions(joinedTransitions);
    dfa.setStates(joinedStates);
    dfa.setAlphabet(alphabet);

    // Generate TFA
    TFA t = dfa.generateTFA();
    printTable(t);

    State *startingL = findStartingState();
    State *startingR = rhs.findStartingState();

    // Return whether the start states are distinguishable
    // or not.
    return !pairCrossed(t, startingL->name, startingR->name);
}

void DFA::printStats(std::ostream &out) const {
    out << "no_of_states=" << states.size() << '\n';

    std::map<char, unsigned int> transitionCount;
    for (const auto &transition : transitions) { ++transitionCount[transition->input]; }

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
void DFA::printStats() const { printStats(std::cout); }
