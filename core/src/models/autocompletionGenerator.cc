#include "autocompletionGenerator.h"

PDFA models::genAutocompletionPDFA(const std::vector<std::string> &keywords,
                                   const std::unordered_map<std::string, unsigned int> &frequencies) {
    // go over all words, add subsets of the words to the set and add new chars to alphabet
    std::set<char> alphabetSet;
    std::set<std::string> subsets;
    for (const auto &keyword : keywords) {
        std::string current;
        for (const char ch : keyword) {
            current += ch;
            subsets.insert(current);

            alphabetSet.insert(ch);
        }
    }

    // insert the symbol for the eps transitions
    alphabetSet.insert('*');
    const std::vector<char> alphabet = {alphabetSet.begin(), alphabetSet.end()};

    // std::cout << "Alphabet:" << '\n';
    // for (const char ch : alphabet) { std::cout << ch; }
    // std::cout << "\n\n";

    // std::cout << "Subset:" << '\n';
    // for (const auto &subset : subsets) {
    //     std::cout << subset << '\n';
    // }

    // build the PDFA
    // insert one starting state that goes to all states with one char
    // keep track of the degree of a state to calculate the weight per transition
    State *startState = new State{.name = "START", .starting = true, .accepting = false};
    std::vector<State *> states = {startState};
    std::vector<Transition *> transitions;
    for (const auto &subset : subsets) {
        // determine if the state is accepting
        const bool accepting = (std::find(keywords.begin(), keywords.end(), subset) != keywords.end());
        State *newState = new State{.name = subset, .starting = false, .accepting = accepting};

        states.push_back(newState);

        if (subset.size() == 1) {
            transitions.push_back(new Transition{.from = startState, .to = newState, .input = subset.front()});
        } else {
            const std::string fromStateName = newState->name.substr(0, newState->name.size() - 1);
            const char input = newState->name.back();

            // find the state to do the transition from
            State *from;
            for (auto state : states) {
                if (state->name == fromStateName) from = state;
            }

            transitions.push_back(new Transition{.from = from, .to = newState, .input = input});
        }
    }

    std::map<const Transition *, double> weights;
    for (const auto transition : transitions) { weights[transition] = 1; }

    // determine the weight for the possible keywords
    // first determine the possible transitions along with the total usage for all transitions from that state
    std::unordered_map<State *, std::vector<std::string>> posTransitions;
    std::unordered_map<const State *, unsigned int> totalUsage;
    for (const auto state : states) {
        if (std::find(keywords.begin(), keywords.end(), state->name) != keywords.end()) continue;

        // check the amount of possible transitions
        for (const auto &keyword : keywords) {
            if (state->name == keyword.substr(0, state->name.size())) {
                posTransitions[state].push_back(keyword);
                totalUsage[state] += frequencies.at(keyword);
            }
        }
    }

    for (const auto &transitionPair : posTransitions) {
        // amount of usage for all keywords in the possible transitions per state
        for (const auto &keyword : transitionPair.second) {
            // usage of this keyword
            const unsigned int usage = (frequencies.find(keyword) == frequencies.end()) ? 0 : frequencies.at(keyword);
            const double weight = (double) usage / (double) totalUsage[transitionPair.first];

            // get the keyword state
            State *to;
            for (const auto state : states) {
                if (state->name == keyword) to = state;
            }

            Transition *newTransition = new Transition{.from = transitionPair.first, .to = to, .input = '*'};
            transitions.push_back(newTransition);
            weights[newTransition] = weight;
        }
    }

    // std::unordered_map<const State *, std::unordered_map<char, double>> totalWeight;
    // for (const auto &weightPair : weights) {
    //     totalWeight[weightPair.first->from][weightPair.first->input] += weightPair.second;
    // }

    // for (const auto &weightPair : totalWeight) {
    //     std::cout << weightPair.first->name << ":\n";
    //     for (const auto &inputPair : weightPair.second) {
    //         std::cout << '\t' << inputPair.first << ": " << inputPair.second << '\n';
    //     }
    // }

    return PDFA{alphabet, states, transitions, weights}.minimize(0.10);
}

void models::genAutocompletionPDFAToFile(const std::vector<std::string> &keywords,
                                         const std::unordered_map<std::string, unsigned int> &frequencies,
                                         std::ostream &out) {
    genAutocompletionPDFA(keywords, frequencies).print(out);
}
