#include <iostream>
#include "crow.h"

std::string generateJSON() {
  // TODO: @Flor remove placeholder for prod
  std::string result = "{ foo: \"bar\" }";

  // TODO: @Achraf syntax designation & error conversion to frontend compatible format
  // result = ...;

  return result;
}

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/json")([](){

    return generateJSON();
  });

  app.port(18080).multithreaded().run();
}