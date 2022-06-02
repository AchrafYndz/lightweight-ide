#include "../lib/path/path.h"
#include "models/autocompletionGenerator.h"
#include "other/cli.h"
#include "other/pythonToEnfa.h"

#include <fstream>
#include <unordered_map>
#include <vector>

int main() {
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

            if (input == "exit()") { break; }

            // ask for a prediction
            std::cout << "Prediction: '" << model.predict() << "'\n";

            // wait for random input before continuing
            std::cin.get();

            // clear console
            cli::clearConsole();
        }
    } else if (action == cli::Action::SyntaxHighlighting) {
        std::string path = cli::waitForPath();
        pythonToEnfa p;
        // generates all automata
        p.generateAutomata(path::rootDirectory + "/src/other/inputfiles/pythonKeyw.txt");
        p.generateDfaFromEnfas(p.enfasKeyw);
        // splits text into tokens and identifies
        p.splitAndIdentify(path);
        // print results (should be commented out once highlighting done)
        p.printIdentifiedTokens(std::cout);

        // highlight based on information in p.keywords, p.comments and p.strings:

        // TODO @Michel: process file for syntax highlighting
    } else if (action == cli::Action::ModelGeneration) {
        // load frequenties from file
        std::ifstream frequentyFile(path::rootDirectory + "/res/frequenties.json");

        if (!frequentyFile.is_open())
            throw std::runtime_error("[ModelGeneration] Cannot open file: '" + path::rootDirectory +
                                     "/res/frequenties.json'");

        // parse the frequenty file
        nlohmann::json j = nlohmann::json::parse(frequentyFile);
        const std::unordered_map<std::string, unsigned int> frequenties = j["keywords"];

        std::ifstream keywordFile(path::rootDirectory + "/res/keywords.txt");
        std::vector<std::string> keywords;

        {
            std::string keyword;
            while (std::getline(keywordFile, keyword)) keywords.push_back(keyword);
        }

        std::ofstream pdfaFile(path::rootDirectory + "/res/autocomplete.json");
        models::genAutocompletionPDFAToFile(keywords, frequenties, pdfaFile);
        pdfaFile.close();
        std::cout << "Model generation complete, model can be found at: '" << path::rootDirectory
                  << "/res/autocomplete.json'\n";
        std::cin.get();

        cli::clearConsole();
    }

    return 0;
}
