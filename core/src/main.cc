#include <iostream>
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
  std::cout << t.peek(9) << std::endl;

  // crow::SimpleApp app;

  // CROW_ROUTE(app, "/json")([](){
  //   crow::response response(generateJSON());
  //   response.set_header("content-type", "application/json");

  //   return response;
  // });

  // app.port(18080).multithreaded().run();
}