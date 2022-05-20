#include "RE.h"

RE::RE(std::string re, const char eps) : eps(eps) {
    // remove whitespace from regex
    re.erase(remove(re.begin(), re.end(), ' '), re.end());
    re.erase(remove(re.begin(), re.end(), '?'), re.end());
    this->re = re;

    for (const char ch : re) {
        if (ch != '(' && ch != ')' && ch != '*' && ch != '+' && ch != eps &&
            std::find(alphabet.begin(), alphabet.end(), ch) == alphabet.end())
            alphabet.push_back(ch);
    }
}

ENFA RE::toENFA() const {
    transitionTable result;
    std::stack<std::pair<std::string, std::pair<std::string, std::string>>> stack;

    unsigned int counter = 0;

    const std::string post = toPostfixExp(re);

    for (const auto &ch : post) {
        // character is a base character
        if (std::find(alphabet.begin(), alphabet.end(), ch) != alphabet.end() || ch == eps) {
            stack.push({std::string() + ch, {}});
        } else if (ch == '*') {
            const std::pair<std::string, std::pair<std::string, std::string>> a = stack.top();
            stack.pop();

            const std::tuple<transitionTable, std::string, std::string> res = generateKleenENFA(a, counter);

            stack.push({"foo", {std::get<1>(res), std::get<2>(res)}});

            for (const auto &pair : std::get<0>(res)) { result[pair.first] = pair.second; }
        } else if (ch == '+') {
            const std::pair<std::string, std::pair<std::string, std::string>> b = stack.top();
            stack.pop();

            const std::pair<std::string, std::pair<std::string, std::string>> a = stack.top();
            stack.pop();

            const std::tuple<transitionTable, std::string, std::string> res = generatePlusENFA(a, b, counter);

            stack.push({"foo", {std::get<1>(res), std::get<2>(res)}});

            for (const auto &pair : std::get<0>(res)) { result[pair.first] = pair.second; }
        } else if (ch == '?') {
            const std::pair<std::string, std::pair<std::string, std::string>> b = stack.top();
            stack.pop();

            const std::pair<std::string, std::pair<std::string, std::string>> a = stack.top();
            stack.pop();

            const std::tuple<transitionTable, std::string, std::string> res = generateConcatenationENFA(a, b, counter);

            stack.push({"foo", {std::get<1>(res), std::get<2>(res)}});

            for (const auto &pair : std::get<0>(res)) { result[pair.first] = pair.second; }
        }
    }

    ENFA enfa;
    enfa.setAlphabet(alphabet);
    enfa.setEpsilon(eps);

    const std::string acceptingState = stack.top().second.second;
    const std::string startingState = stack.top().second.first;

    std::vector<State *> states;
    std::vector<Transition *> transitions;

    for (const auto &statePair : result) {
        const bool starting = (statePair.first == startingState) ? true : false;
        const bool accepting = (statePair.first == acceptingState) ? true : false;
        states.push_back(new State{.name = statePair.first, .starting = starting, .accepting = accepting});
    }
    enfa.setStates(states);

    for (const auto &statePair : result) {
        for (const auto &transitionPair : statePair.second) {
            State *from = enfa.findState(statePair.first);
            for (const auto &toState : transitionPair.second) {
                transitions.push_back(
                  new Transition{.from = from, .to = enfa.findState(toState), .input = transitionPair.first});
            }
        }
    }
    enfa.setTransitions(transitions);

    return enfa;
}

bool RE::precedence(const char a, const char b) const {
    const std::vector<char> operators = {'*', '?', '+'};
    if (std::find(operators.begin(), operators.end(), a) <= std::find(operators.begin(), operators.end(), b))
        return true;
    else
        return false;
}

std::string RE::toPostfixExp(const std::string &re_) const {
    // generate the same regex but with a symbol for concatenation: '?'
    std::string re;
    for (auto it = re_.begin(); it != re_.end(); ++it) {
        re.push_back(*it);

        if (std::next(it) == re_.end()) continue;

        // the current character can be part of a concatenation
        else if (std::find(alphabet.begin(), alphabet.end(), *it) != alphabet.end() || *it == eps || *it == ')' ||
                 *it == '*') {
            const char &next = *std::next(it);

            // the following character is concatenated
            if (std::find(alphabet.begin(), alphabet.end(), next) != alphabet.end() || next == eps || next == '(') {
                re.push_back('?');
            }
        }
    }

    std::string outputQueue;
    std::stack<char> operatorStack;

    for (const auto &ch : re) {
        // character is a base character
        if (std::find(alphabet.begin(), alphabet.end(), ch) != alphabet.end() || ch == eps) {
            outputQueue.push_back(ch);

            // character is an operator
        } else if (ch != '(' && ch != ')') {
            if (operatorStack.empty()) {
                operatorStack.push(ch);
            } else {
                char top = operatorStack.top();

                while (!operatorStack.empty() && top != '(' && precedence(top, ch)) {
                    outputQueue.push_back(top);
                    operatorStack.pop();

                    if (operatorStack.empty()) break;

                    top = operatorStack.top();
                }

                operatorStack.push(ch);
            }

            // character is a bracket
        } else {
            if (ch == '(')
                operatorStack.push(ch);
            else {
                char top = operatorStack.top();

                while (top != '(') {
                    outputQueue.push_back(top);
                    operatorStack.pop();

                    if (operatorStack.empty()) break;

                    top = operatorStack.top();
                }

                operatorStack.pop();
            }
        }
    }

    while (!operatorStack.empty()) {
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

std::tuple<RE::transitionTable, std::string, std::string> RE::generateKleenENFA(
  const std::pair<std::string, std::pair<std::string, std::string>> &a, unsigned int &counter) const {
    transitionTable result;

    std::string firstState;
    std::string lastState;

    // a is a base character
    if (a.first.size() == 1) {
        result["q" + std::to_string(counter)] = {
          {eps, {"q" + std::to_string(counter + 1), "q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 1)] = {{(char) a.first[0], {"q" + std::to_string(counter + 2)}}};
        result["q" + std::to_string(counter + 2)] = {
          {eps, {"q" + std::to_string(counter + 1), "q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 3)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 3);

        counter += 4;

        // a is a generated ENFA
    } else {
        const std::string &firstStateA = a.second.first;
        const std::string &lastStateA = a.second.second;

        result["q" + std::to_string(counter)] = {{eps, {firstStateA, "q" + std::to_string(counter + 1)}}};
        result[lastStateA] = {{eps, {firstStateA, "q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 1);

        counter += 2;
    }

    return {result, firstState, lastState};
}

std::tuple<RE::transitionTable, std::string, std::string> RE::generatePlusENFA(
  const std::pair<std::string, std::pair<std::string, std::string>> &a,
  const std::pair<std::string, std::pair<std::string, std::string>> &b, unsigned int &counter) const {
    transitionTable result;

    std::string firstState;
    std::string lastState;

    // both a and b are base characters
    if (a.first.size() == 1 && b.first.size() == 1) {
        result["q" + std::to_string(counter)] = {
          {eps, {"q" + std::to_string(counter + 1), "q" + std::to_string(counter + 2)}}};
        result["q" + std::to_string(counter + 1)] = {{(char) a.first[0], {"q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 2)] = {{(char) b.first[0], {"q" + std::to_string(counter + 4)}}};
        result["q" + std::to_string(counter + 3)] = {{eps, {"q" + std::to_string(counter + 5)}}};
        result["q" + std::to_string(counter + 4)] = {{eps, {"q" + std::to_string(counter + 5)}}};
        result["q" + std::to_string(counter + 5)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 5);

        counter += 6;

        // one of the two is a base character
    } else if ((a.first.size() == 1 && b.first.size() != 1) || (a.first.size() != 1 && b.first.size() == 1)) {
        std::string firstStateA;
        std::string lastStateA;
        char b_;

        if (a.first.size() == 1) {
            b_ = a.first[0];
            firstStateA = b.second.first;
            lastStateA = b.second.second;
        } else {
            b_ = b.first[0];
            firstStateA = a.second.first;
            lastStateA = a.second.second;
        }

        result["q" + std::to_string(counter)] = {{eps, {firstStateA, "q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {{b_, {"q" + std::to_string(counter + 2)}}};
        result[lastStateA] = {{eps, {"q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 2)] = {{eps, {"q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 3)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 3);

        counter += 4;

        // both are generated ENFAs
    } else {
        result["q" + std::to_string(counter)] = {{eps, {a.second.first, b.second.first}}};
        result[a.second.second] = {{eps, {"q" + std::to_string(counter + 1)}}};
        result[b.second.second] = {{eps, {"q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 1);

        counter += 2;
    }

    return {result, firstState, lastState};
}

std::tuple<RE::transitionTable, std::string, std::string> RE::generateConcatenationENFA(
  const std::pair<std::string, std::pair<std::string, std::string>> &a_,
  const std::pair<std::string, std::pair<std::string, std::string>> &b_, unsigned int &counter) const {
    transitionTable result;

    std::string firstState;
    std::string lastState;

    // both are base characters
    if (a_.first.size() == 1 && b_.first.size() == 1) {
        const char a = a_.first[0];
        const char b = b_.first[0];

        result["q" + std::to_string(counter)] = {{a, {"q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {{eps, {"q" + std::to_string(counter + 2)}}};
        result["q" + std::to_string(counter + 2)] = {{b, {"q" + std::to_string(counter + 3)}}};
        result["q" + std::to_string(counter + 3)] = {};

        firstState = "q" + std::to_string(counter);
        lastState = "q" + std::to_string(counter + 3);

        counter += 4;

        // only b is a generated ENFA
    } else if (a_.first.size() == 1 && b_.first.size() != 1) {
        const std::string firstStateB = b_.second.first;
        const std::string lastStateB = b_.second.second;
        const char a = a_.first[0];

        result["q" + std::to_string(counter)] = {{a, {"q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {{eps, {firstStateB}}};

        firstState = "q" + std::to_string(counter);
        lastState = lastStateB;

        counter += 2;
    } else if (a_.first.size() != 1 && b_.first.size() == 1) {
        const char b = b_.first[0];
        const std::string firstStateA = a_.second.first;
        const std::string lastStateA = a_.second.second;

        result[lastStateA] = {{eps, {"q" + std::to_string(counter)}}};
        result["q" + std::to_string(counter)] = {{b, {"q" + std::to_string(counter + 1)}}};
        result["q" + std::to_string(counter + 1)] = {};

        firstState = firstStateA;
        lastState = "q" + std::to_string(counter + 1);

        counter += 2;
    } else {
        const std::string firstStateA = a_.second.first;
        const std::string lastStateA = a_.second.second;
        const std::string firstStateB = b_.second.first;
        const std::string lastStateB = b_.second.second;

        result[lastStateA] = {{eps, {firstStateB}}};

        firstState = firstStateA;
        lastState = lastStateB;
    }

    return {result, firstState, lastState};
}