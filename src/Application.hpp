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
  PipelineQueue<Producer::StringCoefficients> inputPipeline;
  PipelineQueue<QuadraticEquationSolver::Solution> outputPipeline;

public:
  Application(Producer::Data &data, std::ostream &out)
      : inputData(data), output(out), inputPipeline(MEMORY_SIZE),
        outputPipeline(MEMORY_SIZE) {}

  void run();
};

#endif // APPLICATION_HPP