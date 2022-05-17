#include "PDFA.h"

PDFA::PDFA(const std::string &file) {
    // parse the file
    std::ifstream f(file);
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

        double totalChance = 0;
        for (const auto &chancePair : transition["to"]) {
            State *to = findState(chancePair["state"]);
            const double chance = chancePair["chance"];

            totalChance += chance;

            Transition *transitionParsed = new Transition{.from = from, .to = to, .input = input};
            chances[transitionParsed] = chance;

            transitions.push_back(transitionParsed);
        }

        assert(totalChance == 1);
    }
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
                to.push_back({{"state", target->to->name}, {"chance", chances.at(target)}});

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
        assert(std::find(alphabet.begin(), alphabet.end(), ch) != alphabet.end());

        std::vector<Transition *> posTransitions = findTransition(currentState, ch);

        if (posTransitions.empty()) {
            stuck = true;
            return;
        }

        // select a random weighted transition
        // seed the random number generator
        std::srand((unsigned int) time(nullptr));
        double r = (double) std::rand() / RAND_MAX;
        Transition *chosen;
        for (Transition *transition : posTransitions) {
            double w = chances.at(transition);
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

const State *PDFA::getCurrentState() const { return currentState; }