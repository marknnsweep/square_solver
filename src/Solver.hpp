#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <atomic>
#include <thread>

#include "Producer.hpp"
#include "QuadraticEquationSolver.hpp"
#include "SortOfLockFreeQueue.hpp"

class Solver {
private:
  std::atomic<bool> &input_done;
  std::atomic<bool> &output_done;
  std::atomic<uint32_t> workersDone;

  SortOfLockFreeQueue<Producer::StringCoefficients> &inputQueue;
  SortOfLockFreeQueue<QuadraticEquationSolver::Solution> &outputQueue;

  void done() {
    uint32_t left = workersDone.fetch_sub(1);
    // fetch_sub returns previous value, so 1 means we are the last worker
    if (left == 1) {
      output_done.store(true, std::memory_order_release);
    }
  }

  bool isDone() const {
    // it's important to check both conditions at the same time
    return inputQueue.is_empty() && input_done.load(std::memory_order_acquire);
  }

public:
  Solver(SortOfLockFreeQueue<Producer::StringCoefficients> &inputQueue,
         SortOfLockFreeQueue<QuadraticEquationSolver::Solution> &outputQueue,
         std::atomic<bool> &idf, std::atomic<bool> &odf, uint32_t workers)
      : inputQueue(inputQueue), outputQueue(outputQueue), input_done(idf),
        output_done(odf), workersDone(workers) {}

  void run() {
    Producer::StringCoefficients task;
    QuadraticEquationSolver eq;
    while (true) {
      while (inputQueue.tryDequeue(task)) {
        outputQueue.enqueue(eq.solve(task));
      }
      std::this_thread::yield();

      if (isDone()) {
        break;
      }
    }
    done();
  }
};

#endif // SOLVER_HPP