#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <atomic>
#include <ostream>
#include <thread>

#include "QuadraticEquationSolver.hpp"
#include "SortOfLockFreeQueue.hpp"

class Printer {
private:
  std::atomic<bool> &done_flag;

  std::ostream &output;
  SortOfLockFreeQueue<QuadraticEquationSolver::Solution> &outputQueue;

  bool isDone() const {
    // it's important to check both conditions at the same time
    return outputQueue.is_empty() && done_flag.load(std::memory_order_acquire);
  }

public:
  Printer(std::ostream &output,
          SortOfLockFreeQueue<QuadraticEquationSolver::Solution> &outputQueue,
          std::atomic<bool> &df)
      : output(output), outputQueue(outputQueue), done_flag(df) {}

  void run() {
    QuadraticEquationSolver::Solution task;
    while (true) {
      while (outputQueue.tryDequeue(task)) {
        output << task << std::endl;
      }
      // skip time quant
      std::this_thread::yield();
      if (isDone()) {
        break;
      }
    }
  }
};

#endif // PRINTER_HPP