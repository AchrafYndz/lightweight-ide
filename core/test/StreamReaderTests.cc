#include "doctest/doctest.h"

#include "../src/parser/stream_reader.h"

#include <fstream>
#include <vector>
#include <sstream>

TEST_SUITE("StreamReaderTests") {
  TEST_CASE("[StreamReaderTests] peek") {
    srand((unsigned) time(0)); 
    std::ofstream myfile;
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    myfile.open("stream_reader.txt");
    myfile.clear();

    std::vector<char> written = {};

    for (int _ = 0; _ < 10000; _++) {
      char choice = alphabet[(rand() % 26)];
      written.push_back(choice);
      myfile << choice;
    }

    myfile.close();

    StreamReader reader("stream_reader.txt");
    for (int _ = 0; _ < 10000; _++) {
      int choice = rand() % 10;
      CHECK_EQ(reader.peek(choice), written[choice]);
    }
  }

    TEST_CASE("[StreamReaderTests] consume") {
    srand((unsigned) time(0)); 
    std::ofstream myfile;
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    myfile.open("stream_reader.txt");
    myfile.clear();

    std::vector<char> written = {};

    for (int _ = 0; _ < 10000; _++) {
      char choice = alphabet[(rand() % 26)];
      written.push_back(choice);
      myfile << choice;
    }

    myfile.close();

    StreamReader reader("stream_reader.txt");
    for (int _ = 0; _ < 10000; _++) {
      int choice = rand() % 10;
      CHECK_EQ(reader.consume(choice), written[choice]);
    }
  }
}
