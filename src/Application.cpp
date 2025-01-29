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
  std::atomic<bool> input_flag = false, output_flag = false;
  std::atomic<uint32_t> output_synchro_ctr = solversCount;

  Producer producer(inputData, inputQueue, input_flag);
  Printer printer(output, outputQueue, output_flag);

  Solver solver(inputQueue, outputQueue, input_flag, output_flag, solversCount);
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
  assert(inputQueue.is_empty());
  assert(outputQueue.is_empty());
}