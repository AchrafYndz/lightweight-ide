#include <iostream>
#include <vector>
#include "crow.h"
#include "parser/stream_reader.h"

std::string generateJSON() {
  // TODO: @Flor remove placeholder for prod
  std::string result = "{\"foo\":\"bar\"}";

  // TODO: @Achraf syntax designation & error conversion to frontend compatible format
  // result = ...;

  return result;
}

int main() {
  StreamReader t("t.txt");
  std::cout << t.peek(2) << std::endl;
  std::cout << "Count: " << t.charCount() << std::endl;
  std::cout << t.peek(8) << std::endl;
  std::cout << "Count: " << t.charCount() << std::endl;
  // std::cout << t.peek(9) << std::endl;
  std::cout << "Count: " << t.charCount() << std::endl;
  // std::cout << t.peek(9) << std::endl;
  std::cout << "Count: " << t.charCount() << std::endl;

  // srand((unsigned) time(0)); 
  // std::ofstream myfile;
  // std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
  // myfile.open("stream_reader.txt");
  // myfile.clear();

  // std::vector<char> written = {};

  // for (int _ = 0; _ < 10; _++) {
  //   char choice = alphabet[(rand() % 26)];
  //   written.push_back(choice);
  //   myfile << choice;
  // }

  // myfile.close();

  // StreamReader reader("stream_reader.txt");
  // for (int _ = 0; _ < 10; _++) {
  //   int choice = rand() % 10;
  //   char returned = reader.peek(choice);
  //   std::cout << (returned == written[choice]) << ": " << choice << written[choice] << std::endl;
  // }

  // crow::SimpleApp app;

  // CROW_ROUTE(app, "/json")([](){
  //   crow::response response(generateJSON());
  //   response.set_header("content-type", "application/json");

  //   return response;
  // });

  // app.port(18080).multithreaded().run();
}