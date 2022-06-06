#include "../lib/path/path.h"
#include "models/autocompletionGenerator.h"
#include "other/cli.h"
#include "other/pythonToEnfa.h"

#include <fstream>
#include <stdexcept>
#include <string>
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

            if (input.empty()) {
                cli::clearConsole();
                continue;
            }

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

        std::ifstream sourceFile(path);
        nlohmann::json j;
        std::stringstream tmp("");
        tmp << sourceFile.rdbuf();
        j["code"] = tmp.str();
        j["bounds"] = nlohmann::json::object();
        p.outputTokesAsJson(j["bounds"]);
        // p.printIdentifiedTokens(std::cout);

        std::cout << "Please enter the path you want to store the generated results.\nIt needs to be a relative path "
                     "starting from the root of the project.\n";
        std::string outputPath;
        std::getline(std::cin, outputPath);

        std::ofstream outputFile(path::rootDirectory + '/' + outputPath);

        if (!outputFile.is_open())
            throw std::runtime_error("Could not open file: '" + path::rootDirectory + '/' + outputPath + '\'');

        outputFile << std::setw(4) << j << '\n';
        outputFile.close();

        std::cout << "Succesfully generated results, stored at " << path::rootDirectory << '/' << outputPath << '\n';
        std::cin.get();

        cli::clearConsole();
    } else if (action == cli::Action::ModelGeneration) {
        // load frequencies from file
        std::ifstream frequentyFile(path::rootDirectory + "/res/frequencies.json");

        if (!frequentyFile.is_open())
            throw std::runtime_error("[ModelGeneration] Cannot open file: '" + path::rootDirectory +
                                     "/res/frequencies.json'");

        // parse the frequenty file
        nlohmann::json j = nlohmann::json::parse(frequentyFile);
        const std::unordered_map<std::string, unsigned int> frequencies = j["keywords"];

        std::ifstream keywordFile(path::rootDirectory + "/res/keywords.txt");
        std::vector<std::string> keywords;

        {
            std::string keyword;
            while (std::getline(keywordFile, keyword)) keywords.push_back(keyword);
        }

        std::ofstream pdfaFile(path::rootDirectory + "/res/autocomplete.json");
        models::genAutocompletionPDFAToFile(keywords, frequencies, pdfaFile);
        pdfaFile.close();
        std::cout << "Model generation complete, model can be found at: '" << path::rootDirectory
                  << "/res/autocomplete.json'\n";
        std::cin.get();

        cli::clearConsole();
    }

    return 0;
}

// int main() {
//    PDFA pdfa("../tests/automataTests/res/input/PDFA2.json");
//    PDFA minimized = pdfa.minimize();
//    double total = 0.0;
//    for (auto p: minimized.getWeights()) {
//        std::cout << p.second << "\n";
//        total += p.second;
//    }
//    std::cout << total << std::endl;
//}
