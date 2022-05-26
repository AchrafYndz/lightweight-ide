#ifndef __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_
#define __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_

#include <string>

namespace cli {
    enum Action { Autocomplete, SyntaxHighlighting };

    Action waitForAction();
    std::string waitForPath();
    std::string waitForInput();

    void clearConsole();
}  // namespace cli

#endif  // __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_