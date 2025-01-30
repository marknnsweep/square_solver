#include <algorithm>
#include <atomic>
#include <ostream>
#include <string>
#include <thread>
#include <vector>

#include "Application.hpp"

void Application::run() {
  int32_t solversCount = std::max(
      1, static_cast<int32_t>(std::thread::hardware_concurrency()) - 2);

  PipelineQueue<Producer::StringCoefficients> inputPipeline(MEMORY_SIZE, 1);
  PipelineQueue<QuadraticEquationSolver::Solution> outputPipeline(MEMORY_SIZE,
                                                                  solversCount);

  Producer producer(inputData, inputPipeline);
  Printer printer(output, outputPipeline);

  Solver solver(inputPipeline, outputPipeline);
  std::vector<std::thread> solvers(solversCount);

  auto createSolverThread = [&]() {
    return std::thread([&] { solver.run(); });
  };
  std::thread producerThread([&] { producer.run(); });
  std::generate_n(solvers.begin(), solversCount, createSolverThread);
  std::thread printerThread([&] { printer.run(); });

  producerThread.join();
  std::for_each(solvers.begin(), solvers.end(),
                [](std::thread &t) { t.join(); });
  printerThread.join();
  assert(inputPipeline.isEmpty());
  assert(outputPipeline.isEmpty());
}