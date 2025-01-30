#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Printer.hpp"
#include "Producer.hpp"
#include "Solver.hpp"

class Application {
private:
  static const size_t MEMORY_SIZE = 1024;

  std::ostream &output;
  Producer::Data &inputData;

public:
  Application(Producer::Data &data, std::ostream &out)
      : inputData(data), output(out) {}

  void run();
};

#endif // APPLICATION_HPP