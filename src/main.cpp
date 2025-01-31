#include <iostream>
#include <vector>

#include "Application.hpp"

int main(int argc, char *argv[]) {
  std::vector<std::string> arguments(argv + 1, argv + argc);
  Application app(arguments, std::cout);
  app.run();
  return 0;
}