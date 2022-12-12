#include "crow.h"
#include "parser/stream_reader.h"
#include <iostream>
#include <vector>

#include "machines/CFG.h"
#include "parser/lr.h"

std::string generateJSON() {
    // TODO: @Flor remove placeholder for prod
    std::string result = "{\"foo\":\"bar\"}";

    // TODO: @Achraf syntax designation & error conversion to frontend compatible format
    // result = ...;

    return result;
}

int main() {
    // const CFG cfg("tmp.json");
    // LR lr(cfg);

    // std::cout << "START" << std::endl;
    // StreamReader t("t.txt");
    // std::cout << t.consume(1) << std::endl;
    // std::cout << t.consume(1) << std::endl;
    // std::cout << t.peek(0) << std::endl;
    // std::cout << t.consume(7) << std::endl;
    // std::cout << t.consume(7) << std::endl;
    // std::cout << t.peek(2) << std::endl;
    // std::cout << t.peek(2) << std::endl;
    // std::cout << t.peek(5) << std::endl;
    // std::cout << t.peek(7) << std::endl;

    // srand((unsigned) time(0));
    // std::ofstream myfile;
    // std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    // myfile.open("stream_reader.txt");
    // myfile.clear();
    //
    // std::vector<char> written = {};
    //
    // for (int _ = 0; _ < 10000; _++) {
    //   char choice = alphabet[(rand() % 26)];
    //   written.push_back(choice);
    //   myfile << choice;
    // }
    //
    // myfile.close();
    //
    // StreamReader reader("stream_reader.txt");
    // bool t = false;
    // for (int _ = 0; _ < 10000; _++) {
    //   int choice = rand() % 10;
    //   char returned = reader.consume(choice);
    //   if (returned != written[choice]) t = true;
    //   std::cout << (returned == written[choice]) << ": " << choice << written[choice] << std::endl;
    // }
    //
    // if (t) std::cout << "FAILED" << std::endl;

    // crow::SimpleApp app;

    // CROW_ROUTE(app, "/json")([](){
    //   crow::response response(generateJSON());
    //   response.set_header("content-type", "application/json");

    //   return response;
    // });

    // app.port(18080).multithreaded().run();

    return 0;
}
