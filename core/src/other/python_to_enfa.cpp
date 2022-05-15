#include "python_to_enfa.h"

std::vector<std::string> python_to_enfa::get_python_keyw(const std::string& file) const {
    std::vector<std::string> result;

    std::ifstream infile(file);
    for (std::string line; getline(infile, line);) { result.push_back(line); }

    return result;
}

std::vector<std::pair<char,std::pair<int,int>>> python_to_enfa::scan(const std::string& file) const{
    std::vector<std::pair<char,std::pair<int,int>>> result;

    char ch;
    int i = 0;
    int j = 0;

    std::ifstream infile(file);
    while(infile >> std::noskipws >> ch){
        result.emplace_back(ch, std::make_pair(i,j));
        if (ch == '\n'){
            i = 0;
            j++;
        } else{
            i++;
        }
    }

    return result;
}

void python_to_enfa::scan_to_file(std::ostream &out, const std::string& file) const {
    std::vector<std::pair<char,std::pair<int,int>>> chara = this->scan(file);

    for (unsigned long long int j = 0; j < chara.size(); j++){
        std::string to_print;
        to_print = chara[j].first;
        if (chara[j].first == ' '){
            to_print = "SPACE";
        }
        if (chara[j].first == '\n'){
            to_print = "NEWLINE";
        }

        out << to_print << " " << chara[j].second.first << " " << chara[j].second.second << std::endl;
    }

}