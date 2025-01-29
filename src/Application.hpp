#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Producer.hpp"
#include "Solver.hpp"
#include "Printer.hpp"

class Application {
private:
    static const size_t MEMORY_SIZE = 1024;

    std::ostream& output;
    Producer::Data& inputData;
    SortOfLockFreeQueue<Producer::StringCoefficients> inputQueue;
    SortOfLockFreeQueue<QuadraticEquationSolver::Solution> outputQueue;

public:
    Application(Producer::Data& data, std::ostream& out) : inputData(data), output(out),
              inputQueue(MEMORY_SIZE), outputQueue(MEMORY_SIZE) {}

    void run();
};

#endif // APPLICATION_HPP