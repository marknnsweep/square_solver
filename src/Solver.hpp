#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <atomic>
#include <thread>

#include "PipelineQueue.hpp"
#include "Producer.hpp"
#include "QuadraticEquationSolver.hpp"

class Solver {
private:
  PipelineQueue<Producer::StringCoefficients> &inputPipeline;
  PipelineQueue<QuadraticEquationSolver::Solution> &outputPipeline;

public:
  Solver(PipelineQueue<Producer::StringCoefficients> &inputPipeline,
         PipelineQueue<QuadraticEquationSolver::Solution> &outputPipeline)
      : inputPipeline(inputPipeline), outputPipeline(outputPipeline) {}

  void run() {
    Producer::StringCoefficients task;
    QuadraticEquationSolver eq;
    while (true) {
      while (inputPipeline.tryDequeue(task)) {
        outputPipeline.enqueue(eq.solve(task));
      }
      if (inputPipeline.isDone()) {
        break;
      } else {
        std::this_thread::yield();
      }
    }
    outputPipeline.done();
  }
};

#endif // SOLVER_HPP