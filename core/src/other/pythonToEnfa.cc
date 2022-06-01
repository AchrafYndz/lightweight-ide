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
            if (chars[index].first == '"' and chars[index + 1].first == '"' and chars[index + 2].first == '"' and
                chars[index].second.first != 0) {
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
            if (chars[index].first == '\'' and chars[index + 1].first == '\'' and chars[index + 2].first == '\'' and
                chars[index].second.first != 0) {
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

    // SKIP SEPARATORS
    std::vector<char> separators{'+', '-', '*', '/', '%', '<', '>', '&', '|', '^', '~', '=', '!',
                                 '(', ')', '[', ']', '{', '}', ',', ':', '.', ';', ' ', '\n'};
    index = 0;
    for (it = chars.begin(); it != chars.end(); it++) {
        if (std::find(toSkip.begin(), toSkip.end(), index) != toSkip.end()) {
            index++;
            continue;
        }
        char currentChar = it->first;
        if (std::find(separators.begin(), separators.end(), currentChar) != separators.end()) {
            toSkip.push_back(index);
        }
        index++;
    }

    // WORDS
    for (index = 0; (unsigned long) index < chars.size(); index++) {
        if (std::find(toSkip.begin(), toSkip.end(), index) != toSkip.end()) { continue; }
        std::string word;
        bool endWordFound = false;
        std::pair<int, int> posFirstLetter = std::make_pair(chars[index].second.first, chars[index].second.second);
        std::pair<int, int> posLastLetter;

        while (!endWordFound) {
            word += chars[index].first;
            posLastLetter = std::make_pair(chars[index].second.first, chars[index].second.second);
            if ((unsigned long) index == chars.size() - 1) {
                endWordFound = true;
            } else if (std::find(toSkip.begin(), toSkip.end(), index + 1) != toSkip.end()) {
                endWordFound = true;
            }
            index++;
        }
        result[std::make_pair(posFirstLetter, posLastLetter)] = word;
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

std::vector<char> pythonToEnfa::getSigma() const {
    std::vector<char> result;
    // note: i = 32 is space
    for (int i = 33; i <= 127; i++) {
        // skip +, (, ), * and ?
        if (i == 43 or i == 40 or i == 41 or i == 42 or i == 63) { continue; }
        result.push_back((char) i);
    }
    result.push_back('\n');
    return result;
}

void pythonToEnfa::removeElementsFromSet(std::vector<char> &inputSet, char ch) const {
    std::vector<char>::iterator position = std::find(inputSet.begin(), inputSet.end(), ch);
    if (position != inputSet.end()) { inputSet.erase(position); }
}

std::string pythonToEnfa::expand(std::vector<char> &inputSet) const {
    std::string result;

    std::vector<char>::iterator it;
    for (it = inputSet.begin(); it != inputSet.end() - 1; it++) {
        result += *it;
        result += "+";
    }
    result += inputSet.back();

    return result;
}

std::string pythonToEnfa::replaceRegexOp(const std::string &str) const {
    std::string result;
    result = str;
    std::replace(result.begin(), result.end(), '+', 'a');
    std::replace(result.begin(), result.end(), '(', 'a');
    std::replace(result.begin(), result.end(), ')', 'a');
    std::replace(result.begin(), result.end(), '*', 'a');
    std::replace(result.begin(), result.end(), '?', 'a');
    return result;
}


std::vector<ENFA> pythonToEnfa::generateEnfaKeywords(const std::string &file) const {
    std::vector<ENFA> result;
    std::vector<std::string> keyw = getPythonKeyw(file);
    std::vector<std::string>::iterator it;
    for (it = keyw.begin(); it != keyw.end(); it++) {
        RE reg(*it, '%');
        ENFA enfa = reg.toENFA();
        result.push_back(enfa);
    }
    return result;
}

std::vector<ENFA> pythonToEnfa::generateEnfaComments() const {
    std::vector<ENFA> result;

    std::string regexStr;
    std::vector<char> sigma = getSigma();
    regexStr += "\n'''(";
    regexStr += expand(sigma);
    regexStr += ")*'''";
    RE reg(regexStr, ' ');
    ENFA enfa = reg.toENFA();
    result.push_back(enfa);

    std::string regexStr2;
    regexStr2 += "\n\"\"\"(";
    regexStr2 += expand(sigma);
    regexStr2 += ")*\"\"\"";
    RE reg2(regexStr2, ' ');
    ENFA enfa2 = reg2.toENFA();
    result.push_back(enfa2);

    std::string regexStr3;
    regexStr3 += "#(";
    regexStr3 += expand(sigma);
    regexStr3 += ")*";
    RE reg3(regexStr3, ' ');
    ENFA enfa3 = reg3.toENFA();
    result.push_back(enfa3);

    return result;
}

std::vector<ENFA> pythonToEnfa::generateEnfaStrings() const {
    std::vector<ENFA> result;

    std::string regexStr;
    std::vector<char> sigma = getSigma();
    removeElementsFromSet(sigma, '"');
    removeElementsFromSet(sigma, '\n');
    regexStr += "\"(";
    regexStr += expand(sigma);
    regexStr += ")*\"";
    RE reg(regexStr, ' ');
    ENFA enfa = reg.toENFA();
    result.push_back(enfa);

    std::string regexStr2;
    sigma = getSigma();
    removeElementsFromSet(sigma, '\'');
    removeElementsFromSet(sigma, '\n');
    regexStr2 += "\'(";
    regexStr2 += expand(sigma);
    regexStr2 += ")*\'";
    RE reg2(regexStr2, ' ');
    ENFA enfa2 = reg2.toENFA();
    result.push_back(enfa2);

    std::string regexStr3;
    sigma = getSigma();
    regexStr3 += "\"\"\"(";
    regexStr3 += expand(sigma);
    regexStr3 += ")*\"\"\"";
    RE reg3(regexStr3, ' ');
    ENFA enfa3 = reg3.toENFA();
    result.push_back(enfa3);

    std::string regexStr4;
    regexStr4 += "'''(";
    regexStr4 += expand(sigma);
    regexStr4 += ")*'''";
    RE reg4(regexStr4, ' ');
    ENFA enfa4 = reg4.toENFA();
    result.push_back(enfa4);

    return result;
}

bool pythonToEnfa::isKeyword(const std::string &str, std::vector<ENFA> &enfaKeyw) const {
    bool result = false;

    std::vector<ENFA>::iterator it;
    for (it = enfaKeyw.begin(); it != enfaKeyw.end(); it++){
        if(it->accepts(str)){
            result = true;
        }
    }

    return result;
}

bool pythonToEnfa::isComment(const std::string &str, std::vector<ENFA> &enfaComments) const {
    bool result = false;

    std::string strNoOp = replaceRegexOp(str);
    std::vector<ENFA>::iterator it;
    for (it = enfaComments.begin(); it != enfaComments.end(); it++){
        if(it->accepts(strNoOp)){
            result = true;
        }
    }

    return result;
}

bool pythonToEnfa::isString(const std::string &str, std::vector<ENFA> &enfaStrings) const {
    bool result = false;

    std::string strNoOp = replaceRegexOp(str);
    std::vector<ENFA>::iterator it;
    for (it = enfaStrings.begin(); it != enfaStrings.end(); it++){
        if(it->accepts(strNoOp)){
            result = true;
        }
    }

    return result;
}


void pythonToEnfa::identifyTokens(const std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> &m) {
    // OLD METHOD USES CONTAINERS

    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string>::const_iterator it;

    for (it = m.begin(); it!= m.end(); it++){
        std::string str = it->second;

        bool identified = false;
        bool isaKeyw;
        bool isaComment;
        bool isaString;


        std::pair<pos,pos> positions;

        isaKeyw = isKeyword(str, enfasKeyw);
        if (isaKeyw){
            positions = it->first;
            keywords.push_back(positions);
            identified = true;
        }
        if(!identified){
            isaComment = isComment(str, enfasComments);
            if (isaComment){
                positions = it->first;
                comments.push_back(positions);
                identified = true;
            }
        }
        if(!identified){
            isaString = isString(str, enfasStrings);
            if (isaString){
                positions = it->first;
                strings.push_back(positions);
            }
        }
    }
    /*

    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string>::const_iterator it;

    std::string regexStr;
    std::vector<char> sigma = getSigma();
    removeElementsFromSet(sigma, '"');
    removeElementsFromSet(sigma, '\n');
    regexStr += "\"(";
    regexStr += expand(sigma);
    regexStr += ")*\"";
    RE reg(regexStr, ' ');
    ENFA enfaStr1 = reg.toENFA();

    std::string regexStr2;
    sigma = getSigma();
    removeElementsFromSet(sigma, '\'');
    removeElementsFromSet(sigma, '\n');
    regexStr2 += "\'(";
    regexStr2 += expand(sigma);
    regexStr2 += ")*\'";
    RE reg2(regexStr2, ' ');
    ENFA enfaStr2 = reg2.toENFA();

    std::string regexStr3;
    sigma = getSigma();
    regexStr3 += "\"\"\"(";
    regexStr3 += expand(sigma);
    regexStr3 += ")*\"\"\"";
    RE reg3(regexStr3, ' ');
    ENFA enfaStr3 = reg3.toENFA();

    std::string regexStr4;
    regexStr4 += "'''(";
    regexStr4 += expand(sigma);
    regexStr4 += ")*'''";
    RE reg4(regexStr4, ' ');
    ENFA enfaStr4 = reg4.toENFA();

    std::string regexStr5;
    sigma = getSigma();
    regexStr5 += "\n'''(";
    regexStr5 += expand(sigma);
    regexStr5 += ")*'''";
    RE reg5(regexStr5, ' ');
    ENFA enfaCom1 = reg5.toENFA();

    std::string regexStr6;
    regexStr6 += "\n\"\"\"(";
    regexStr6 += expand(sigma);
    regexStr6 += ")*\"\"\"";
    RE reg6(regexStr6, ' ');
    ENFA enfaCom2 = reg6.toENFA();

    std::string regexStr7;
    regexStr7 += "#(";
    regexStr7 += expand(sigma);
    regexStr7 += ")*";
    RE reg7(regexStr7, ' ');
    ENFA enfaCom3 = reg7.toENFA();

    for (it = m.begin(); it != m.end(); it++) {
        std::string str = it->second;

        bool identified = false;
        bool isaKeyw = false;

        std::pair<pos, pos> positions;
        std::vector<std::string> keyw = getPythonKeyw(fileKeyw);
        std::vector<std::string>::iterator it2;

        for (it2 = keyw.begin(); it2 != keyw.end(); it2++) {
            RE rege(*it2, '%');
            ENFA enfa = rege.toENFA();
            if (enfa.accepts(str)) { isaKeyw = true; }
        }
        if (isaKeyw) {
            positions = it->first;
            keywords.push_back(positions);
            identified = true;
        }
        if (!identified) {
            std::string strNoOp = replaceRegexOp(str);
            if (enfaCom1.accepts(strNoOp) or enfaCom2.accepts(strNoOp) or enfaCom3.accepts(strNoOp)) {
                positions = it->first;
                comments.push_back(positions);
                identified = true;
            }
        }
        if (!identified) {
            std::string strNoOp = replaceRegexOp(str);
            if (enfaStr1.accepts((strNoOp)) or enfaStr2.accepts((strNoOp)) or enfaStr3.accepts((strNoOp)) or
                enfaStr4.accepts((strNoOp))) {
                positions = it->first;
                strings.push_back(positions);
                identified = true;
            }
        }
    }
     */
}

void pythonToEnfa::printIdentifiedTokens(std::ostream &out) const {
    std::vector<std::pair<pos, pos>>::const_iterator it;

    if (keywords.empty()) {
        out << "There are no keywords." << std::endl;
    } else {
        out << "Keywords:" << std::endl;
        for (it = keywords.begin(); it != keywords.end(); it++) {
            out << "(" << it->first.first << ", " << it->first.second << ") "
                << "(" << it->second.first << ", " << it->second.second << ")" << std::endl;
        }
    }

    if (comments.empty()) {
        out << "There are no comments." << std::endl;
    } else {
        out << "Comments:" << std::endl;
        for (it = comments.begin(); it != comments.end(); it++) {
            out << "(" << it->first.first << ", " << it->first.second << ") "
                << "(" << it->second.first << ", " << it->second.second << ")" << std::endl;
        }
    }

    if (strings.empty()) {
        out << "There are no strings." << std::endl;
    } else {
        out << "Strings:" << std::endl;
        for (it = strings.begin(); it != strings.end(); it++) {
            out << "(" << it->first.first << ", " << it->first.second << ") "
                << "(" << it->second.first << ", " << it->second.second << ")" << std::endl;
        }
    }
}

void pythonToEnfa::generateDfaFromEnfas(const std::vector<ENFA> &enfas) {
    // FUNCTION DOES NOT WORK CORRECTLY (error in product automaton?)
    // convert ENFAS to DFAS
    std::vector<DFA> dfas;
    for(auto& enfa : enfas){
        dfas.push_back(enfa.toDFA());
    }
    unsigned int size = dfas.size();
    // if only one dfa present
    if (size == 1){
        dfaKeyw = dfas[0];
    }
    else{
        // if more than one dfa present
        DFA curDFA = dfas[0];
        for(unsigned int i = 1; i < size; i++){
            curDFA = DFA(curDFA,dfas[i], false);
        }
        dfaKeyw = curDFA;
    }
}
void pythonToEnfa::generateAutomata(const std::string &file) {
    enfasKeyw = generateEnfaKeywords(file);
    enfasComments = generateEnfaComments();
    enfasStrings = generateEnfaStrings();
}
void pythonToEnfa::splitAndIdentify(const std::string &file) {
    identifyTokens(splitText(file));
}
