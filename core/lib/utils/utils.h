#ifndef IDE_UTILS_H
#define IDE_UTILS_H

#include <stdexcept>
#include <string>

namespace utils {
inline int stoi(const std::string& s) {
        int result = 0;
        int factor = 1;
        for (std::string::const_reverse_iterator it = s.rbegin(); it != s.rend(); ++it) {
            if (it == s.rend() + 1 && *it == 45) {
                result *= -1;
            } else if (!(48 <= *it && *it <= 57)) {
                throw std::runtime_error("[Utils::stoi] Conversion error, cannot convert '" + s + "' to an integer.");
            } else {
                result += (*it - '0') * factor;
                factor *= 10;
            }
        }
        return result;
    }

}

#endif // IDE_UTILS_H
