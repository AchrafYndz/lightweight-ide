#include "cli.h"
#include <iostream>

namespace cli {
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
       
       
    }
}
