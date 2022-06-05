#ifndef __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_
#define __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_

#include "../../lib/path/path.h"
#include "../../lib/utils/utils.h"

#include <exception>
#include <iostream>
#include <string>

namespace cli {
    enum Action { Autocomplete, SyntaxHighlighting, ModelGeneration };

    Action waitForAction();
    std::string waitForPath();
    std::string waitForInput();

    void clearConsole();
}  // namespace cli

#endif  // __PROJECTS_LIGHTWEIGHT_IDE_CORE_SRC_OTHER_CLI_H_
