#include "pythonToEnfa.h"

std::vector<std::string> pythonToEnfa::getPythonKeyw(const std::string &file) const {
    std::vector<std::string> result;

    std::ifstream infile(file);
    for (std::string line; getline(infile, line);) result.push_back(line);

    return result;
}

std::vector<std::pair<char, std::pair<int, int>>> pythonToEnfa::scan(const std::string &file) const {
    std::vector<std::pair<char, std::pair<int, int>>> result;

    char ch;
    int i = 0, j = 0;

    // read the file char by char without skipping whitespace
    std::ifstream infile(file);
    while (infile >> std::noskipws >> ch) {
        result.emplace_back(ch, std::make_pair(i, j));
        if (ch == '\n') {
            i = 0;
            j++;
        } else
            i++;
    }

    return result;
}

void pythonToEnfa::scanToFile(std::ostream &out, const std::string &file) const {
    std::vector<std::pair<char, std::pair<int, int>>> chara = this->scan(file);

    for (unsigned long j = 0; j < chara.size(); j++) {
        std::string toPrint;
        toPrint = chara[j].first;
        if (chara[j].first == ' ') { toPrint = "SPACE"; }
        if (chara[j].first == '\n') { toPrint = "NEWLINE"; }

        out << toPrint << " " << chara[j].second.first << " " << chara[j].second.second << '\n';
    }
}

std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> pythonToEnfa::splitText(
  const std::string &file) const {
    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> result;
    std::vector<std::pair<char, std::pair<int, int>>> chars = scan(file);

    int index = 0;
    int charsSize = (int) chars.size();
    std::vector<int> toSkip;
    std::vector<std::pair<char, std::pair<int, int>>>::iterator it;
    for (it = chars.begin(); it != chars.end(); it++) {
        if (std::find(toSkip.begin(), toSkip.end(), index) != toSkip.end()) {
            index++;
            continue;
        }

        // SINGLE LINE COMMENTS
        if (it->first == '#') {
            int row = it->second.second;
            int column = it->second.first;

            int quoteDoubleCount = 0;
            int quoteSingleCount = 0;
            bool quoteDoubleMatched = true;
            bool quoteSingleMatched = true;
            std::vector<std::pair<char, std::pair<int, int>>>::iterator it2;
            for (it2 = chars.begin(); it2 != chars.end(); it2++) {
                if (it2->second.second == row and it2->second.first < column and it2->first == '"') {
                    if (quoteSingleMatched) { quoteDoubleCount++; }
                    if (quoteDoubleCount % 2 != 0) {
                        quoteDoubleMatched = false;
                    } else {
                        quoteDoubleMatched = true;
                    }
                }
                if (it2->second.second == row and it2->second.first < column and it2->first == '\'') {
                    if (quoteDoubleMatched) { quoteSingleCount++; }
                    if (quoteSingleCount % 2 != 0) {
                        quoteSingleMatched = false;
                    } else {
                        quoteSingleMatched = true;
                    }
                }
            }

            if (quoteDoubleMatched and quoteSingleMatched) {
                std::string comment;

                int indexToSkip = 0;
                std::vector<std::pair<char, std::pair<int, int>>>::iterator it3;
                for (it3 = chars.begin(); it3 != chars.end(); it3++) {
                    if (it3->second.second == row and it3->second.first >= column and it3->first != '\n') {
                        comment += it3->first;
                        toSkip.push_back(indexToSkip);
                    }
                    indexToSkip++;
                }
                int strLength = (int) comment.length();
                result[std::make_pair(std::make_pair(it->second.first, it->second.second),
                                      std::make_pair(it->second.first + strLength - 1, it->second.second))] = comment;
            }
        }

        // MULTILINE COMMENTS with "
        if (index < charsSize - 5) {
            if (chars[index].first == '"' and chars[index + 1].first == '"' and chars[index + 2].first == '"' and
                chars[index].second.first == 0) {
                std::pair<int, int> startP = std::make_pair(chars[index].second.first, chars[index].second.second);
                std::string multiLComment = "\n";
                multiLComment += R"(""")";
                toSkip.push_back(index);
                toSkip.push_back(index + 1);
                toSkip.push_back(index + 2);
                int indexToSkip = index + 3;

                while (indexToSkip < charsSize - 2) {
                    if (chars[indexToSkip].first == '"' and chars[indexToSkip + 1].first == '"' and
                        chars[indexToSkip + 2].first == '"') {
                        break;
                    }
                    if (std::find(toSkip.begin(), toSkip.end(), indexToSkip) == toSkip.end()) {
                        toSkip.push_back(indexToSkip);
                    }
                    multiLComment += chars[indexToSkip].first;
                    indexToSkip++;
                }
                multiLComment += R"(""")";
                toSkip.push_back(indexToSkip);
                toSkip.push_back(indexToSkip + 1);
                toSkip.push_back(indexToSkip + 2);

                indexToSkip = indexToSkip + 2;
                while (indexToSkip < charsSize) {
                    if (chars[indexToSkip + 1].first != '"') {
                        break;
                    } else {
                        multiLComment += '"';
                        toSkip.push_back(indexToSkip + 1);
                    }
                    indexToSkip++;
                }
                std::pair<int, int> endP =
                  std::make_pair(chars[indexToSkip].second.first, chars[indexToSkip].second.second);
                result[std::make_pair(startP, endP)] = multiLComment;
            }
        }

        // MULTILINE COMMENTS with '
        if (index < charsSize - 5) {
            if (chars[index].first == '\'' and chars[index + 1].first == '\'' and chars[index + 2].first == '\'' and
                chars[index].second.first == 0) {
                std::pair<int, int> startP = std::make_pair(chars[index].second.first, chars[index].second.second);
                std::string multiLComment = "\n";
                multiLComment += R"(''')";
                toSkip.push_back(index);
                toSkip.push_back(index + 1);
                toSkip.push_back(index + 2);
                int indexToSkip = index + 3;

                while (indexToSkip < charsSize - 2) {
                    if (chars[indexToSkip].first == '\'' and chars[indexToSkip + 1].first == '\'' and
                        chars[indexToSkip + 2].first == '\'') {
                        break;
                    }
                    if (std::find(toSkip.begin(), toSkip.end(), indexToSkip) == toSkip.end()) {
                        toSkip.push_back(indexToSkip);
                    }
                    multiLComment += chars[indexToSkip].first;
                    indexToSkip++;
                }
                multiLComment += R"(''')";
                toSkip.push_back(indexToSkip);
                toSkip.push_back(indexToSkip + 1);
                toSkip.push_back(indexToSkip + 2);

                indexToSkip = indexToSkip + 2;
                while (indexToSkip < charsSize) {
                    if (chars[indexToSkip + 1].first != '\'') {
                        break;
                    } else {
                        multiLComment += '\'';
                        toSkip.push_back(indexToSkip + 1);
                    }
                    indexToSkip++;
                }
                std::pair<int, int> endP =
                  std::make_pair(chars[indexToSkip].second.first, chars[indexToSkip].second.second);
                result[std::make_pair(startP, endP)] = multiLComment;
            }
        }

        // MULTILINE STRINGS with "
        if (index < charsSize - 5) {
            if (chars[index].first == '"' and chars[index + 1].first == '"' and chars[index + 2].first == '"') {
                std::pair<int, int> startP = std::make_pair(chars[index].second.first, chars[index].second.second);
                std::string multiLComment = R"(""")";
                toSkip.push_back(index);
                toSkip.push_back(index + 1);
                toSkip.push_back(index + 2);
                int indexToSkip = index + 3;

                while (indexToSkip < charsSize - 2) {
                    if (chars[indexToSkip].first == '"' and chars[indexToSkip + 1].first == '"' and
                        chars[indexToSkip + 2].first == '"') {
                        break;
                    }
                    if (std::find(toSkip.begin(), toSkip.end(), indexToSkip) == toSkip.end()) {
                        toSkip.push_back(indexToSkip);
                    }
                    multiLComment += chars[indexToSkip].first;
                    indexToSkip++;
                }
                multiLComment += R"(""")";
                toSkip.push_back(indexToSkip);
                toSkip.push_back(indexToSkip + 1);
                toSkip.push_back(indexToSkip + 2);

                indexToSkip = indexToSkip + 2;
                while (indexToSkip < charsSize) {
                    if (chars[indexToSkip + 1].first != '"') {
                        break;
                    } else {
                        multiLComment += '"';
                        toSkip.push_back(indexToSkip + 1);
                    }
                    indexToSkip++;
                }
                std::pair<int, int> endP =
                  std::make_pair(chars[indexToSkip].second.first, chars[indexToSkip].second.second);
                result[std::make_pair(startP, endP)] = multiLComment;
            }
        }

        // MULTILINE STRINGS with '
        if (index < charsSize - 5) {
            if (chars[index].first == '\'' and chars[index + 1].first == '\'' and chars[index + 2].first == '\'') {
                std::pair<int, int> startP = std::make_pair(chars[index].second.first, chars[index].second.second);
                std::string multiLComment = R"(''')";
                toSkip.push_back(index);
                toSkip.push_back(index + 1);
                toSkip.push_back(index + 2);
                int indexToSkip = index + 3;

                while (indexToSkip < charsSize - 2) {
                    if (chars[indexToSkip].first == '\'' and chars[indexToSkip + 1].first == '\'' and
                        chars[indexToSkip + 2].first == '\'') {
                        break;
                    }
                    if (std::find(toSkip.begin(), toSkip.end(), indexToSkip) == toSkip.end()) {
                        toSkip.push_back(indexToSkip);
                    }
                    multiLComment += chars[indexToSkip].first;
                    indexToSkip++;
                }
                multiLComment += R"(''')";
                toSkip.push_back(indexToSkip);
                toSkip.push_back(indexToSkip + 1);
                toSkip.push_back(indexToSkip + 2);

                indexToSkip = indexToSkip + 2;
                while (indexToSkip < charsSize) {
                    if (chars[indexToSkip + 1].first != '\'') {
                        break;
                    } else {
                        multiLComment += '\'';
                        toSkip.push_back(indexToSkip + 1);
                    }
                    indexToSkip++;
                }
                std::pair<int, int> endP =
                  std::make_pair(chars[indexToSkip].second.first, chars[indexToSkip].second.second);
                result[std::make_pair(startP, endP)] = multiLComment;
            }
        }

        // SINGLE LINE STRINGS
        if (it->first == '"' or it->first == '\'') {
            char ch;
            if (it->first == '"') {
                ch = '"';
            } else {
                ch = '\'';
            }
            std::pair<int, int> posFirst = std::make_pair(it->second.first, it->second.second);
            std::pair<int, int> posLast;
            int indexesToSkip = index;

            std::string strContent;
            strContent += ch;
            toSkip.push_back(indexesToSkip);

            std::string str_content;
            bool matchingQuoteFound = false;
            while (!matchingQuoteFound) {
                indexesToSkip++;
                char curChar = chars[indexesToSkip].first;
                strContent += curChar;
                toSkip.push_back(indexesToSkip);
                if (curChar == ch) {
                    matchingQuoteFound = true;
                    posLast = std::make_pair(chars[indexesToSkip].second.first, chars[indexesToSkip].second.second);
                }
            }
            result[std::make_pair(posFirst, posLast)] = strContent;
        }

        index++;
    }
    return result;
}

void pythonToEnfa::printMap(std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> &m,
                            std::ostream &out) const {
    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string>::iterator it;
    for (it = m.begin(); it != m.end(); it++) {
        int column1 = it->first.first.first;
        int row1 = it->first.first.second;
        int column2 = it->first.second.first;
        int row2 = it->first.second.second;
        std::string str = it->second;
        out << "(" << column1 << ", " << row1 << ")"
            << " "
            << "(" << column2 << ", " << row2 << ")"
            << " " << str << '\n';
    }
}
