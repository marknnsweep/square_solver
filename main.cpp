#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <memory>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <cctype>
#include <optional>

template<typename IntType, typename FloatType>
class QuadraticEquation {
private:
    IntType a, b, c;

public:
    struct Solution {
        FloatType root1;
        FloatType root2;
        FloatType xmin;
        bool no_roots = false;
        bool no_real_roots = false;
        bool no_xmin = false;
        bool incorrect_input = false;
    };

    QuadraticEquation(int a, int b, int c) : a(a), b(b), c(c) {}

    Solution solve() const {
        Solution solution;

        if (a == 0) {
            if (b == 0) {
                solution.incorrect_input = true;
                return solution;
            } else {
                FloatType root = -static_cast<FloatType>(c) / b;
                solution.root1 = root;
                solution.no_xmin = true;
                return solution;
            }
        }

        IntType discriminant = static_cast<IntType>(b) * b - (static_cast<IntType>(a) * c << 2);

        if (discriminant < 0) {
            solution.xmin = -static_cast<FloatType>(b) / (2 * a);
            solution.no_roots = true;
            solution.no_real_roots = true;
            return solution;
        }

        IntType sqrt_d = static_cast<IntType>(sqrt(discriminant));
        if (sqrt_d * sqrt_d != discriminant) {
            solution.xmin = -static_cast<FloatType>(b) / (2 * a);
            solution.no_real_roots = true;
            return solution;
        }

        IntType divisor = 2 * a;
        FloatType root1 = (-b + sqrt_d) / static_cast<FloatType>(divisor);
        FloatType root2 = (-b - sqrt_d) / static_cast<FloatType>(divisor);
        solution.root1 = root1;
        solution.root2 = root2;
        solution.xmin = -static_cast<FloatType>(b) / (2 * a);

        return solution;
    }

    friend std::ostream& operator<<(std::ostream& os, const Solution& solution) {
        if (solution.incorrect_input) {
            os << "(inf, inf) Xmin=inf";
        } else if (solution.no_xmin) {
            os << "(" << solution.root1 << ", inf) Xmin=inf";
        } else if (solution.no_roots) {
            os << "(nan, nan) Xmin=" << solution.xmin;
        } else {
            os << "(" << solution.root1 << ", " << solution.root2 << ") Xmin=" << solution.xmin;
        }
        return os;
    }
};

class InputData {
public:
    using Coefficients = std::tuple<int, int, int>;

    class Iterator {
    private:
        std::vector<std::string>::iterator current;
        std::vector<std::string>::iterator end;
        Coefficients current_tuple;

    public:
        Iterator(std::vector<std::string>::iterator start, std::vector<std::string>::iterator finish) : current(start), end(finish) {
            advance();
        }

        Coefficients operator*() const {
            return current_tuple;
        }

        Iterator& operator++() {
            ++current;
            advance();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

    private:
        static std::optional<int> parseInteger(const std::string& s) {
            try {
                return std::stoi(s);
            } catch (...) {
                return std::nullopt;
            }
        }

        void advance() {
            if (current == end) {
                return;
            }

            int a = 0, b = 0, c = 0;
            int i = 0;
            while (i < 3 && current != end) {
                auto value = parseInteger(*current);
                if (value) {
                    if (i == 0) a = *value;
                    else if (i == 1) b = *value;
                    else if (i == 2) c = *value;
                    ++i;
                }
                ++current;
            }

            current_tuple = std::make_tuple(a, b, c);
        }
    };

private:
    std::vector<std::string> arguments;

public:
    InputData(char* argv[], int argc) : arguments(argv + 1, argv + argc) {}

    Iterator begin() {
        return Iterator(arguments.begin(), arguments.end());
    }

    Iterator end() {
        return Iterator(arguments.end(), arguments.end());
    }
};

class Task {
public:
    virtual ~Task() = default;
    virtual void execute() = 0;
};

class SolveTask : public Task {
private:
    int a, b, c;
    QuadraticEquation<int, double>::Solution solution;

public:
    SolveTask(int a, int b, int c) : a(a), b(b), c(c) {}

    void execute() override {
        QuadraticEquation<int, double> equation(a, b, c);
        solution = equation.solve();
    }

    QuadraticEquation<int, double>::Solution getSolution() const {
        return solution;
    }
};

class OutputTask : public Task {
private:
    QuadraticEquation<int, double>::Solution solution;

public:
    OutputTask(const QuadraticEquation<int, double>::Solution& solution) : solution(solution) {}

    void execute() override {
    }
};

template<T>
class CQueue {
private:
  std::queue<T> queue;

public:
    CQueue() {
    }

    void enqueue(T task);

    bool dequeue(T& task);
    
    void setDone();
};

class Application {
private:
    std::ostream& output;
    InputData& inputData;
    std::queue<SolveTask> solveTaskQueue;
    std::queue<OutputTask> outputTaskQueue;

public:
    Application(InputData& data, std::ostream& out) : inputData(data), output(out) {}

    void produce() {
        for (auto it = inputData.begin(); it != inputData.end(); ++it) {
            const auto& [a, b, c] = *it;
            solveTaskQueue.emplace(a, b, c);
        }
    }

    void processQueue(std::queue<TaskType>& taskQueue) {
        while (!taskQueue.empty()) {
            TaskType task = std::move(taskQueue.front());
            taskQueue.pop();
            task.execute(output);
        }
    }
    
    void consumeSolve() {
        processQueue(solveTaskQueue);
    }
    
    void consumeOutput() {
        processQueue(outputTaskQueue);
    }

    void run() {
        produce();
        consumeSolve();
        consumeOutput();
    }
};


int main(int argc, char* argv[]) {
    InputData data(argv, argc);
    Application app(data, std::cout);
    app.run();
    return 0;
}