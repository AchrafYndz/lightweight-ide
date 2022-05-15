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
std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> python_to_enfa::split_text(
  const std::string &file) const {

    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> result;
    std::vector<std::pair<char,std::pair<int,int>>> chars = scan(file);

    // SINGLE LINE COMMENTS
    int index = 0;
    std::vector<std::pair<char,std::pair<int,int>>>::iterator it;
    for (it = chars.begin(); it !=chars.end(); it++){
        if (it->first == '#'){
            int row = it->second.second;
            int column = it->second.first;

            int quote_double_count = 0;
            int quote_single_count = 0;
            bool quote_double_matched = true;
            bool quote_single_matched = true;
            std::vector<std::pair<char,std::pair<int,int>>>::iterator it2;
            for (it2 = chars.begin(); it2 !=chars.end(); it2++){
                if (it2->second.second == row and it2->second.first < column and it2->first == '"'){
                    if (quote_single_matched){
                        quote_double_count++;
                    }
                    if (quote_double_count%2 != 0){
                        quote_double_matched = false;
                    } else {
                        quote_double_matched = true;
                    }
                }
                if (it2->second.second == row and it2->second.first < column and it2->first == '\''){
                    if (quote_double_matched){
                        quote_single_count++;
                    }
                    if (quote_single_count%2 != 0){
                        quote_single_matched = false;
                    } else{
                        quote_single_matched = true;
                    }
                }

            }

            if (quote_double_matched and quote_single_matched){
                std::string comment;


                std::vector<std::pair<char,std::pair<int,int>>>::iterator it3;
                for (it3 = chars.begin(); it3 !=chars.end(); it3++){
                    if(it3->second.second == row and it3->second.first >= column and it3->first != '\n'){
                        comment += it3->first;
                    }
                }
                int str_length = (int) comment.length();
                result[std::make_pair(std::make_pair(it->second.first, it->second.second),std::make_pair(it->second.first, it->second.second + str_length))] = comment;
            }
        }
        index++;
    }
    return result;
}

void python_to_enfa::print_map(std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> &m,
                               std::ostream &out) const {
    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string>::iterator it;
    for (it = m.begin(); it != m.end(); it++){
        int column_1 =  it->first.first.first;
        int row_1 = it->first.first.second;
        int column_2 =  it->first.second.first;
        int row_2 = it->first.second.second;
        std::string str = it->second;
        out << "(" << column_1 <<", " << row_1 << ")" << " " << "(" << column_2 <<", " << row_2 << ")" << " " << str << std::endl;
    }
}
