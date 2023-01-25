#include "crow.h"
#include "nlohmann/json.hpp"

#include "machines/CFG.h"
#include "parser/lexer.h"
#include "parser/lr.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

std::string generateJSON(const LR& lr, const std::string& filepath, const std::string& request_source) {
    nlohmann::json result{};
    result["code"] = request_source;

    const LR::ParseResult parse_result = lr.parse(StreamReader(filepath));
    if (!parse_result.success) {
        for (auto error : parse_result.errors)
            result["errors"].push_back({{"line", error.line},
                                        {"message", std::move(error.message)},
                                        {"corrections", std::move(error.corrections)}});
    }

    if (parse_result.tree.has_value()) {
        std::cout << "ASTree content: \n" << parse_result.tree.value()->getContent();
    } else {
        std::cout << "Cannot print ASTree for program with unrecoverable errors." << std::endl;
    }

    std::cout << "Parse result: " << ((parse_result.success) ? "success" : "failed") << '\n';

    // delete file from system
    if (!std::filesystem::remove(filepath))
        throw std::runtime_error("failed to delete temporary source file");

    // TODO: @Achraf analyze parse tree for syntax highlighting
    // identifier
    // function_call_identifier, function_name, arguments
    // keywords
    // literals
    // comments
    // punctuation (= brackets and operators)

    if (parse_result.tree.has_value()) {
        const auto& root = parse_result.tree.value()->getRoot();
        root->inorderVisit(root, result["bounds"]);
    } else {
        result["bounds"] = {};
    }

    std::cout << std::setw(4) << result << std::endl;

    // TODO: @Flor remove placeholder for prod
    // std::string result =
    //     R"~({"code":"from string import ascii_lowercase\\n\\ndef main():\\n    # This is a comment!\\n    message =
    //     \"Hello, alphabet!\"\\n    print(message,
    //     ascii_lowercase)\\n\\nmain()","bounds":{"strings":[[87,105]],"comments":[[52,72]],"keywords":[[0,4],[12,18],[36,39]]},"errors":[{"line":0,"message":"Expected
    //     module, got 'string'"}]})~";

    // TODO: @Achraf syntax designation & error conversion to frontend compatible format
    // result = ...;

    return result.dump();
}

/// Returns string with escaped characters replaced with actual characters.
/// Heavily isnpired by (this)[https://tomeko.net/online_tools/cpp_unescape.php] tool.
std::string raw_to_string(const std::string& code) {
    std::string result{};
    enum class State { Default, Escape };
    State state{State::Default};

    unsigned int i{0};
    for (i = 0; i < code.length(); ++i) {
        switch (state) {
        case State::Default:
            switch (code[i]) {
            case '\"':
                std::runtime_error(std::string() +
                                   "Non escaped `\"` not allowed in string. At position: " + std::to_string(i));
                break;
            case '\\':
                state = State::Escape;
                break;
            default:
                result += code[i];
                break;
            }
            break;

        case State::Escape:
            switch (code[i]) {
            case 'f':
                result += '\f';
                break;
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            case '"':
                result += '\"';
                break;
            case '\\':
                result += '\\';
                break;

            default:
                throw std::runtime_error(
                    std::string() + "Unexpected character (" + code[i] +
                    "), expecting one of the escaped character (f, n, r, t, \\, or quotation). At position: " +
                    std::to_string(i));
                return result;
            }
            state = State::Default;
            break;
        }
    }

    if (state != State::Default) {
        throw std::runtime_error(std::string() +
                                 "Source string not terminated correctly. At position: " + std::to_string(i));
    }

    return result;
}

std::string move_code_into_file(const std::string& code) {
    // get current id of the thread to store source file
    const std::thread::id thread_id = std::this_thread::get_id();

    std::string filename = "tmp/" + (std::stringstream() << thread_id).str();

    std::ofstream source_file(filename);
    // Note: `raw_to_string` is only needed when the source code is a raw string literal. It can handle both.
    source_file << raw_to_string(code);
    source_file.close();

    return filename;
}

int main() {
    const CFG cfg = CFG::parse_ebnf("res/grammar/paithon.gram");
    LR lr(cfg);

    // const std::pair<bool, LR::ASTree*> result = lr.parse(StreamReader("factorial.pai"));
    //
    // if (result.second) {
    //     std::cout << "ASTree content: \n" << result.second->getContent();
    // } else {
    //     std::cout << "Cannot generate AST for program that contains to many errors." << std::endl;
    // }
    //
    // std::cout << "Parse result: " << ((result.first) ? "success" : "failed") << '\n';

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() { return "Hello, world!"; });

    CROW_ROUTE(app, "/json").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        // parse request body as json
        nlohmann::json body = nlohmann::json::parse(req.body);

        // move source code into source file to free memory
        const std::string filename = move_code_into_file(body["code"]);

        crow::response response(generateJSON(lr, filename, body["code"]));
        response.set_header("content-type", "application/json");
        response.set_header("Access-Control-Allow-Origin", "*");

        return response;
    });

    app.port(18080).multithreaded().run();

    return 0;
}
