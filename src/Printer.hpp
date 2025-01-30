#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <atomic>
#include <ostream>
#include <thread>

#include "QuadraticEquationSolver.hpp"
#include "PipelineQueue.hpp"

class Printer {
private:
  std::ostream &output;
  PipelineQueue<QuadraticEquationSolver::Solution> &outputPipeline;

public:
  Printer(std::ostream &output,
          PipelineQueue<QuadraticEquationSolver::Solution> &outputPipeline)
      : output(output), outputPipeline(outputPipeline) {}

  void run() {
    QuadraticEquationSolver::Solution task;
    while (true) {
      while (outputPipeline.tryDequeue(task)) {
        output << task << std::endl;
      }

      if (outputPipeline.isDone()) {
         break;
      } else  {
        // skip time quant
        std::this_thread::yield();
      }
    }
  }
};

#endif // PRINTER_HPP