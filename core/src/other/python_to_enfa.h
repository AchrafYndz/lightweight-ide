#ifndef IDE_PYTHON_TO_ENFA_H
#define IDE_PYTHON_TO_ENFA_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

class python_to_enfa {
  public:
    std::vector<std::string> get_python_keyw(const std::string& file) const; // reads python keywords from file and places them in vector

    // reads all characters from file, store them in vector with row and column number
    std::vector<std::pair<char,std::pair<int,int>>> scan(const std::string& file) const;
    void scan_to_file(std::ostream &out,const std::string& file) const;
    // splits text in potential tokens
    std::map< std::pair<std::pair<int,int>, std::pair<int,int>> , std::string> split_text(const std::string& file) const;
    void print_map(std::map< std::pair<std::pair<int,int>, std::pair<int,int>> , std::string>& m, std::ostream& out) const;

};

#endif  // IDE_PYTHON_TO_ENFA_H

