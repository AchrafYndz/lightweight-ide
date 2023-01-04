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

std::string generateJSON(const LR& lr, const std::string& filepath) {
    std::pair<bool, LR::ASTree*> parse_result = lr.parse(StreamReader(filepath));

    std::cout << "Parse result: " << ((parse_result.first) ? "success" : "failed") << '\n';

    // delete file from system
    if (!std::filesystem::remove(filepath))
        throw std::runtime_error("failed to delete temporary source file");

    // TODO: analyze parse tree for syntax highlighting

    // TODO: @Flor remove placeholder for prod
    std::string result =
        R"~({"code":"from string import ascii_lowercase\\n\\ndef main():\\n    # This is a comment!\\n    message = \"Hello, alphabet!\"\\n    print(message, ascii_lowercase)\\n\\nmain()","bounds":{"strings":[[87,105]],"comments":[[52,72]],"keywords":[[0,4],[12,18],[36,39]]},"errors":[{"line":0,"message":"Expected module, got 'string'"}]})~";

    // TODO: @Achraf syntax designation & error conversion to frontend compatible format
    // result = ...;

    delete parse_result.second;

    return result;
}

/// Returns string with escaped characters replaced with actual characters.
/// Heavily isnpired by (this)[https://tomeko.net/online_tools/cpp_unescape.php] tool.
std::string raw_to_string(const std::string& code) {
    std::string result{};
    enum class State { OPENING, STRING, ESCAPING };
    State state{State::OPENING};

    unsigned int i{0};
    for (i = 0; i < code.length(); ++i) {
        switch (state) {
        case State::OPENING:
            switch (code.at(i)) {
            case ' ':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
                break;
            case '\"':
                state = State::STRING;
                break;
            default:
                if (i != code.length() - 1) {
                    throw std::runtime_error(std::string() + "Unexpected character (" + code[i] +
                                             "), expecting quotation mark. At position: " + std::to_string(i));
                    return result;
                } else {
                    throw std::runtime_error(std::string() + "Unexpected character (" + code[i] +
                                             "), conversion interrupted. At position: " + std::to_string(i));
                    break;
                }
            }
            break;

        case State::STRING:
            switch (code[i]) {
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                break;
            case '\"':
                state = State::OPENING;
                break;
            case '\\':
                state = State::ESCAPING;
                break;
            default:
                result += code[i];
                break;
            }
            break;

        case State::ESCAPING:
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
            state = State::STRING;
            break;
        }
    }

    if (state != State::OPENING) {
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
    source_file << raw_to_string(code);
    source_file.close();

    return filename;
}

int main() {
    const CFG cfg = CFG::parse_ebnf("res/grammar/brolang.gram");
    LR lr(cfg);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() { return "Hello, world!"; });

    CROW_ROUTE(app, "/json").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        // move source code into source file to free memory
        const std::string filename = move_code_into_file(req.body);

        crow::response response(generateJSON(lr, filename));
        response.set_header("content-type", "application/json");
        response.set_header("Access-Control-Allow-Origin", "*");

        return response;
    });

    app.port(18080).multithreaded().run();

    return 0;
}
