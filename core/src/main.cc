#include "crow.h"
#include "parser/stream_reader.h"
#include <iostream>
#include <string>
#include <vector>

#include "machines/CFG.h"
#include "parser/lr.h"

std::string generateJSON(const std::string& code) {
    // TODO: @Flor remove placeholder for prod
    std::string result = "{\"foo\":\"bar\"}";

    // TODO: @Achraf syntax designation & error conversion to frontend compatible format
    // result = ...;

    return result;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
      return "Hello, world!";
    });

    CROW_ROUTE(app, "/json").methods(crow::HTTPMethod::POST)([](const crow::request& req){
      crow::response response(generateJSON(req.body));
      response.set_header("content-type", "application/json");

      return response;
    });

    app.port(18080).multithreaded().run();

    return 0;
}
