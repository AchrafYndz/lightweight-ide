#include "../lib/path/path.h"
#include "models/autocompletionGenerator.h"
#include "other/cli.h"

#include <fstream>
#include <unordered_map>
#include <vector>

int main() {
    // const std::unordered_map<std::string, unsigned int> frequenties = {
    //   {"assert", 74064}, {"import", 63923}, {"def", 59620},     {"await", 58217}, {"from", 53931},  {"None", 52338},
    //   {"return", 34893}, {"async", 29284},  {"if", 29244},      {"True", 18886},  {"False", 11808}, {"with", 9007},
    //   {"as", 8660},      {"is", 8133},      {"not", 7191},      {"in", 6719},     {"class", 6616},  {"for", 5998},
    //   {"except", 4799},  {"try", 3717},     {"and", 3520},      {"raise", 3081},  {"or", 2619},     {"elif", 2483},
    //   {"lambda", 1430},  {"else", 1258},    {"continue", 1131}, {"yield", 932},   {"del", 384},     {"break", 320},
    //   {"pass", 245},     {"while", 238},    {"nonlocal", 183},  {"finally", 133}, {"global", 18}};

    // std::ifstream keywordFile(path::rootDirectory + "/res/keywords.txt");
    // std::vector<std::string> keywords;

    // {
    //     std::string keyword;
    //     while (std::getline(keywordFile, keyword)) keywords.push_back(keyword);
    // }

    // std::ofstream pdfaFile(path::rootDirectory + "/res/autocomplete.json");
    // models::genAutocompletionPDFAToFile(keywords, frequenties, pdfaFile);

    // CLI
    cli::Action action = cli::waitForAction();

    if (action == cli::Action::Autocomplete) {
        while (true) {
            // load the autocompletion models
            PDFA model(path::rootDirectory + "/res/autocomplete.json");
            
            std::cin.clear(), std::cin.sync();
            std::string input = cli::waitForInput();

            // feed the data to the model
            model.input(input);

            // ask for a prediction
            std::cout << "Prediction: '" << model.predict() << "'\n";

            // wait for random input before continuing
            std::cin.get();

            // Clear console
            cli::clearConsole();
        }
    } else if (action == cli::Action::SyntaxHighlighting) {
        std::string path = cli::waitForPath();

        // TODO @Michel: process file for syntax highlighting
    }

    return 0;
}
