#include "cli.h"
#include <cstdio>
#include <iostream>

namespace cli {
    void clearConsole() {
        std::cout << "\033[2J\033[1;1H";
    }

    Action waitForAction() {
        // Wait for input
        std::cout << "#######################################################" << std::endl;
        std::cout << "# Please enter the number corresponding               #" << std::endl;
        std::cout << "# to the feature you'd like to use followed by enter  #" << std::endl;
        std::cout << "#######################################################" << std::endl;
        std::cout << std::endl;
        std::cout << "\x1B[32m[0]\033[0m Autocomplete" << std::endl;
        std::cout << "\x1B[32m[1]\033[0m Syntax highlighting" << std::endl;
        std::cout << std::endl;

        std::cout << "\x1B[32m>>\033[0m ";

        // Get user input
        int x;
        scanf("%d",&x);

        // Clear console
        clearConsole();

        // process input
        if (x == 0) return Action::Autocomplete;
        else if (x == 1) return Action::SyntaxHighlighting;

        throw "Invalid input";
    }

    std::string waitForPath() {
        // Wait for input
        std::cout << "###############################################################################" << std::endl;
        std::cout << "# Please enter the path of the source file                                    #" << std::endl;
        std::cout << "# to perform syntax highlighting on followed by enter (default ./input.py)    #" << std::endl;
        std::cout << "###############################################################################" << std::endl;
        std::cout << std::endl;
        std::cout << "\x1B[32m>>\033[0m ";

        // Get user input
        std::string path;
        getline(std::cin, path);

        // Clear console
        clearConsole();

        // Process path
        if ((path[0] == '\'' || path[0] == '"') && (path[path.length() - 1] == '\'' || path[path.length() - 1] == '"')) path = path.substr(1, path.length() - 2);
        if (path.length() == 0) path = "./input.py";
        return path;
    }
}
