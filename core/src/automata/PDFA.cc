#include "PDFA.h"

PDFA::PDFA(const std::string &file) {
    // parse the file
    std::ifstream f(file);

    if (!f.is_open()) throw std::runtime_error("[PDFA] Could not load file: '" + file + '\'');

    const nlohmann::json j = nlohmann::json::parse(f);

    // check the automata type
    assert(j["type"] == "PDFA");

    // parse alphabet
    for (const auto &ch : j["alphabet"]) alphabet.push_back(std::string(ch).front());

    // parse states
    for (const auto &state : j["states"]) {
        State *stateParsed =
          new State{.name = state["name"], .starting = state["starting"], .accepting = state["accepting"]};

        states.push_back(stateParsed);
    }

    // parse transitions and chances
    for (const auto &transition : j["transitions"]) {
        State *from = findState(transition["from"]);
        const char input = std::string(transition["input"]).front();

        for (const auto &chancePair : transition["to"]) {
            State *to = findState(chancePair["state"]);
            const double chance = chancePair["chance"];

            Transition *transitionParsed = new Transition{.from = from, .to = to, .input = input};
            weights[transitionParsed] = chance;

            transitions.push_back(transitionParsed);
        }
    }

    currentState = findStartingState();
}

PDFA::PDFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
           const std::vector<Transition *> &transitions, const std::map<const Transition *, double> &weights) :
    FA(alphabet, states, transitions), weights(weights) {
    currentState = findStartingState();
}

void PDFA::print(std::ostream &out) const {
    // Initialize json object
    nlohmann::json j;

    std::vector<std::string> stringAlphabet;
    for (const char ch : alphabet) { stringAlphabet.push_back(std::string() + ch); }

    // convert chances and transitions back to json
    nlohmann::json parsedTransitions;
    for (const auto &state : states) {
        for (const char ch : alphabet) {
            std::vector<Transition *> posTransitions = findTransition(state, ch);

            if (posTransitions.empty()) continue;

            nlohmann::json to;
            for (const auto &target : posTransitions)
                to.push_back({{"state", target->to->name}, {"chance", weights.at(target)}});

            parsedTransitions.push_back({{"from", state->name}, {"input", std::string() + ch}, {"to", to}});
        }
    }

    // Add all properties
    j["type"] = "PDFA";
    j["alphabet"] = stringAlphabet;
    j["states"] = states;
    j["transitions"] = parsedTransitions;

    // Print
    out << std::setw(4) << j << '\n';
}

void PDFA::input(const std::string &in) {
    if (stuck) return;

    // if the PDFA isn't initialised yet, start it by setting the current state to the startSate
    if (!currentState) currentState = findStartingState();

    // assert that a startState is found
    assert(currentState != nullptr);

    for (const char ch : in) {
        // assert that the input char is part of the alphabet
        // assert(std::find(alphabet.begin(), alphabet.end(), ch) != alphabet.end());
        if (std::find(alphabet.begin(), alphabet.end(), ch) == alphabet.end()) return;

        std::vector<Transition *> posTransitions = findTransition(currentState, ch);

        if (posTransitions.empty()) {
            if (ch == '*') {
                return;
            } else {
                stuck = true;
                currentState = nullptr;
                return;
            }
        }

        // select a random weighted transition
        // seed the random number generator
        std::srand((unsigned int) time(nullptr));
        double r = (double) (std::rand() % 100000) / (double) 100000;
        Transition *chosen;
        for (Transition *transition : posTransitions) {
            double w = weights.at(transition);
            r -= w;
            if (r < 0) {
                chosen = transition;
                break;
            }

            // set the currentTransition as a fail-safe
            chosen = transition;
        }

        currentState = chosen->to;
    }
}

std::string PDFA::predict() {
    if (stuck || currentState->name == "START") return "";

    // because we do not want to update the PDFA, store the old state and reset it later
    State *curr = currentState;

    // input the 'epsilon' char
    input("*");

    const std::string result = currentState->name;
    currentState = curr;

    return result;
}

const std::map<const Transition *, double> &PDFA::getWeights() const { return weights; }

const State *PDFA::getCurrentState() const { return currentState; }

PDFA PDFA::minimize(const double threshold) {
    std::vector<char> minAlphabet = alphabet;
    std::vector<State *> minStates = {};
    std::vector<Transition *> minTransitions = {};

    std::map<const Transition *, double> minWeights = {};
    std::map<const Transition *, double> minMinWeights = {};
    std::unordered_map<std::string, State *> newStates;

    for (std::pair<const Transition *, double> p : weights) {
        // Ignore transitions with low weights
        if (p.second < threshold) continue;
        // Determine minimized states
        if (find(minStates.begin(), minStates.end(), p.first->from) == minStates.end()) {
            State *newState = p.first->from->copy();
            newStates[p.first->from->name] = newState;
            minStates.push_back(newState);
        } 
        if (find(minStates.begin(), minStates.end(), p.first->to) == minStates.end()) {
            State *newState = p.first->to->copy();
            newStates[p.first->to->name] = newState;
            minStates.push_back(newState);
        }
        // Add all other transitions
        minWeights.insert(p);
    }

    // Fix the weights
    for (State *s : minStates) {
        std::map<const Transition *, double> curWeights;
        for (std::pair<const Transition *, double> p : minWeights) {
            if (p.first->from == s) curWeights.insert(p);
        }
        // Calculate total weight
        double total = 0.0;
        for (std::pair<const Transition *, double> p : curWeights) { total += p.second; }
        // Rebalance to 1
        for (std::pair<const Transition *, double> p : curWeights) {
            double newWeight = p.second / total;
            minWeights[p.first] = newWeight;
        }
    }

    // Push all the transitions from weights to the transitions vector
    minTransitions.reserve(minWeights.size());
    for (auto &p : minWeights) {
        State *from = newStates.at(p.first->from->name);
        State *to = newStates.at(p.first->to->name);
        Transition *newTransition = new Transition{.from = from, .to = to, .input = p.first->input};

        minTransitions.push_back(newTransition);
        minMinWeights[newTransition] = p.second;
    }

    return PDFA(alphabet, minStates, minTransitions, minMinWeights);
}
