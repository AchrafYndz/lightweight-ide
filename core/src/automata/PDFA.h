#ifndef IDE_PDFA_H
#define IDE_PDFA_H

#include "FA.h"

#include "../../lib/nlohmann-json/json.hpp"

#include <ctime>

class PDFA : public FA {
    // Pair of a Transiton and a chance
    std::map<const Transition *, double> weights;

    State *currentState = nullptr;
    bool stuck = false;

  public:
    PDFA() = default;
    PDFA(const std::string &filename);
    PDFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
         const std::vector<Transition *> &transitions, const std::map<const Transition *, double> &weights);

    void print(std::ostream &out) const;

    void input(const std::string &in);

    const State *getCurrentState() const;

    const std::map<const Transition *, double> &getWeights() const;

    std::string predict();
};

#endif  // IDE_PDFA_H
