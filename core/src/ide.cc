#include "../lib/path/path.h"
#include "models/autocompletionGenerator.h"
#include "other/cli.h"

#include <fstream>
#include <unordered_map>
#include <vector>

int main() {
    // CLI
    cli::Action action = cli::waitForAction();

    if (action == cli::Action::Autocomplete) {
        const std::unordered_map<std::string, unsigned int> frequenties = {
          {"if", 842},     {"return", 682}, {"def", 578},   {"elif", 193},    {"is", 183},    {"for", 155},
          {"import", 114}, {"from", 92},    {"class", 90},  {"and", 83},      {"in", 67},     {"not", 60},
          {"else", 48},    {"del", 44},     {"while", 40},  {"continue", 33}, {"or", 33},     {"pass", 33},
          {"False", 30},   {"None", 30},    {"True", 30},   {"finally", 30},  {"global", 30}, {"nonlocal", 30},
          {"raise", 30},   {"break", 24},   {"lambda", 15}, {"assert", 10},   {"try", 9},     {"except", 9},
          {"with", 3},     {"as", 3},       {"yield", 1}};

        // read python keywords from input file to build the pdfa from
        std::ifstream keywordsFile(path::rootDirectory + "/res/keywords.txt");

        std::vector<std::string> keywords;

        // exit the program with an error if the file is not opened
        if (!keywordsFile.is_open()) return 1;

        // anomimous scope (to prevent shadowing of variable 'keyword'
        {
            std::string keyword;
            while (std::getline(keywordsFile, keyword)) { keywords.push_back(keyword); }
        }

        std::ofstream pdfaFile(path::rootDirectory + "/res/autocompletion.json");
        models::genAutocompletionPDFAToFile(keywords, frequenties, pdfaFile);

        while (true) { 
            std::string input = cli::waitForInput();
            // TODO @Jonas: process input for autocomplete
        }
    } else if (action == cli::Action::SyntaxHighlighting) {
        std::string path = cli::waitForPath();

        // TODO @Michel: process file for syntax highlighting
    }

    return 0;
}
