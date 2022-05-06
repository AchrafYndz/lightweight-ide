#include "path.h"
#include <string>

const std::string path::rootDirectory = getRootPath();

std::string path::getRootPath() {
    const std::string currentPath = simplify(__FILE__);
    const unsigned int rootDirStringIndex = reverseFindNth(currentPath, '/', 2);

    return currentPath.substr(0, rootDirStringIndex);
}

/// Finds the nth occurence of a character starting from the back
/// Returns '-1' if it can't find the nth occurence
/// Note: the 'n' is zero based (because we are real programmers :p )
unsigned int path::reverseFindNth(const std::string &str, char ch, unsigned int n) {
    int index = str.size() - 1;
    unsigned int nth = 0;
    for (std::string::const_reverse_iterator it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == ch) {
            if (nth == n)
                return index;
            else
                ++nth;
        }

        --index;
    }

    return -1;
}

std::string path::simplify(const std::string &path) {
    // absolute path

    // using vector in place of stack
    std::vector<std::string> v;
    int n = path.length();
    std::string ans;
    for (int i = 0; i < n; i++) {
        std::string dir = "";
        // forming the current directory.
        while (i < n && path[i] != '/') {
            dir += path[i];
            i++;
        }

        // if ".." , we pop.
        if (dir == "..") {
            if (!v.empty()) v.pop_back();
        } else if (dir == "." || dir == "") {
            // do nothing (added for better understanding.)
        } else {
            // push the current directory into the vector.
            v.push_back(dir);
        }
    }

    // forming the ans
    for (auto i : v) { ans += "/" + i; }

    // vector is empty
    if (ans == "") return "/";

    return ans;
}