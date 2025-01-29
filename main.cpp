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
#include <vector>
#include <tuple>
#include <optional>
#include <iterator>
#include <type_traits>
#include <stdexcept>
#include <tuple>
#include <optional>
#include <iterator>
#include <type_traits>
#include <stdexcept>
#include <atomic>
#include <assert.h>

class QuadraticEquationSolver {
public:
    using Coefficients = std::tuple<std::optional<int64_t>, std::optional<int64_t>, std::optional<int64_t>>;
    using Data = std::tuple<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>>;
private:
    static std::optional<int64_t> parseInteger(const std::string& s) {
        try {
            return std::stoi(s);
        } catch (...) {
            return std::nullopt;
        }
    }

public:

    struct Solution {
        Solution() = default;
        
        Solution(Coefficients coeff) : coefficients(coeff), incorrect_input(true) {}
        Solution(Coefficients coeff, double _xmin) : coefficients(coeff), xmin(_xmin), no_roots(true), no_real_roots(true) {}
        Solution(Coefficients coeff, double root, bool) : coefficients(coeff), root1(root), no_xmin(true) {}
        Solution(Coefficients coeff, double _xmin, double root) : coefficients(coeff), xmin(_xmin), root1(root) {}
        Solution(Coefficients coeff, double _xmin, double _root1, double _root2) : coefficients(coeff), xmin(_xmin), root1(_root1), root2(_root2) {}

        Coefficients coefficients;
        std::optional<double> root1;
        std::optional<double> root2;
        std::optional<double> xmin;
        bool no_roots = false;
        bool no_real_roots = false;
        bool no_xmin = false;
        bool incorrect_input = false;
    };

    // Constructor that converts vector of string tuples to Coefficients
    QuadraticEquationSolver() = default;

    Solution solve(const Data& stringCoeff) const {

        Coefficients coefficients = parseCoefficietns(stringCoeff);
        // Check if all coefficients are valid
        if (!std::get<0>(coefficients).has_value() ||
            !std::get<1>(coefficients).has_value() ||
            !std::get<2>(coefficients).has_value()) {
            return Solution(coefficients);
        }

        int64_t a = std::get<0>(coefficients).value();
        int64_t b = std::get<1>(coefficients).value();
        int64_t c = std::get<2>(coefficients).value();

        if (a == 0) {
            if (b == 0) {
                return Solution(coefficients);
            } else {
                double root = -static_cast<double>(c) / b;
                // single root, no xmin
                return Solution(coefficients, root, true);
            }
        }

        double sum = -static_cast<double>(b) / a;
        double product = static_cast<double>(c) / a;

        double discriminant = sum * sum - 4 * product;
        if (discriminant < 0) {
            // no roots, xmin exists
            return Solution(coefficients, -sum / 2);
        }

        double sqrt_d = std::sqrt(discriminant);
        double root1 = (sum + sqrt_d) / 2;
        double root2 = (sum - sqrt_d) / 2;
        double xmin = -sum / 2;

        return Solution(coefficients, root1, root2, xmin);
    }

    friend std::ostream& operator<<(std::ostream& os, const Solution& solution) {
        const auto& [a, b, c] = solution.coefficients;

        // Output coefficients with missing values as empty spaces
        os << "(";
        os << (a.has_value() ? std::to_string(a.value()) : "");
        os << ",";
        os << (b.has_value() ? std::to_string(b.value()) : "");
        os << ",";
        os << (c.has_value() ? std::to_string(c.value()) : "");
        os << ") => ";

        // Output based on solution correctness
        if (solution.incorrect_input) {
            os << "invalid input";
        } else {
            os << "(";
            os << (solution.root1.has_value() ? std::to_string(solution.root1.value()) : "nan");
            os << ", ";
            os << (solution.root2.has_value() ? std::to_string(solution.root2.value()) : "nan");
            os << ") Xmin=";
            os << (solution.xmin.has_value() ? std::to_string(solution.xmin.value()) : "nan");
        }

        return os;
    }
private:
    static Coefficients parseCoefficietns(const Data& stringCoefficients) {
        const auto& [a_str, b_str, c_str] = stringCoefficients;
        return std::make_tuple(
            a_str ? parseInteger(*a_str) : std::nullopt,
            b_str ? parseInteger(*b_str) : std::nullopt,
            c_str ? parseInteger(*c_str) : std::nullopt
            );
    }
};

template <typename T>
class SortOfLockFreeQueue {
  unsigned int buffer_size;

  struct CellTy {
    std::atomic<unsigned> sequence;
    T data;
  };

  alignas(64) std::vector<CellTy> buffer;
  alignas(64) unsigned buffer_mask;
  alignas(64) std::atomic<unsigned> enqueue_pos, dequeue_pos;

public:
  SortOfLockFreeQueue(unsigned int buffer_size)
      : buffer(buffer_size), buffer_mask(buffer_size - 1) {
    for (unsigned i = 0; i != buffer_size; ++i)
      buffer[i].sequence.store(i, std::memory_order_relaxed);

    enqueue_pos.store(0, std::memory_order_relaxed);
    dequeue_pos.store(0, std::memory_order_relaxed);
  }

  bool tryEnqueue(T data) {
    CellTy *cell;
    unsigned pos;
    bool res = false;

    while (!res) {
      pos = enqueue_pos.load(std::memory_order_relaxed);
      cell = &buffer[pos & buffer_mask];
      auto seq = cell->sequence.load(std::memory_order_acquire);
      auto diff = static_cast<int>(seq) - static_cast<int>(pos);

      if (diff < 0)
        return false;

      if (diff == 0)
        res = enqueue_pos.compare_exchange_weak(pos, pos + 1,
                                               std::memory_order_relaxed);
    }

    cell->data = std::move(data);
    cell->sequence.store(pos + 1, std::memory_order_release);
    return true;
  }

  void enqueue(T data) {
    while (!tryEnqueue(data)) {
       std::this_thread::yield();
    }
  }

  template <typename... Args>
  void enqueue(Args&&... args) {
    enqueue(T(std::forward<Args>(args)...));
  }


  bool tryDequeue(T &data) {
    CellTy *cell;
    unsigned pos;
    bool res = false;

    while (!res) {
      pos = dequeue_pos.load(std::memory_order_relaxed);
      cell = &buffer[pos & buffer_mask];
      auto Seq = cell->sequence.load(std::memory_order_acquire);
      auto diff = static_cast<int>(Seq) - static_cast<int>(pos + 1);

      if (diff < 0)
        return false;

      if (diff == 0)
        res = dequeue_pos.compare_exchange_weak(pos, pos + 1,
                                               std::memory_order_relaxed);
    }

    data = std::move(cell->data);
    cell->sequence.store(pos + buffer_mask + 1, std::memory_order_release);
    return true;
  }

  bool is_empty() const { return enqueue_pos.load() == dequeue_pos.load(); }
};

// Template class for producing tuples of a fixed size from a collection
template <typename Collection, unsigned Size, typename = std::enable_if_t<(Size > 0)>>
class TupleProducer {
public:
    using Iterator = typename Collection::iterator;
    using ValueType = typename Collection::value_type;

    // Generate TupleType with Size elements of type std::optional<ValueType>
    template <std::size_t... Indexes>
    static auto create_empty_tuple(std::index_sequence<Indexes...>) {
        return std::make_tuple((static_cast<void>(Indexes), std::optional<ValueType>{})...);
    }

    using TupleType = decltype(create_empty_tuple(std::make_index_sequence<Size>{}));

    class IteratorWrapper {
    private:
        Iterator current;
        Iterator end;
        TupleType current_tuple;

    public:
        explicit IteratorWrapper(Iterator start, Iterator finish) : current(start), end(finish) {
            advance();
        }

        const TupleType& operator*() const {
            return current_tuple;
        }

        IteratorWrapper& operator++() {
            ++current;
            advance();
            return *this;
        }

        bool operator!=(const IteratorWrapper& other) const {
            return current != other.current;
        }

    private:
        void advance() {
            if (current == end) {
                return;
            }

            std::vector<std::optional<ValueType>> elements;
            for (unsigned i = 0; i < Size && current != end; ++i, ++current) {
                elements.push_back(*current);
            }

            --current;

            while (elements.size() < Size) {
                elements.push_back(std::nullopt);
            }

            current_tuple = to_tuple(elements);
        }

        static TupleType to_tuple(const std::vector<std::optional<ValueType>>& elements) {
            return create_tuple(elements, std::make_index_sequence<Size>{});
        }

        template <std::size_t... Indexes>
        static TupleType create_tuple(const std::vector<std::optional<ValueType>>& elements, std::index_sequence<Indexes...>) {
            return std::make_tuple((Indexes < elements.size() ? elements[Indexes] : std::nullopt)...);
        }
    };

private:
    Collection collection;

public:
    explicit TupleProducer(const Collection& coll) : collection(coll) {}

    IteratorWrapper begin() {
        return IteratorWrapper(collection.begin(), collection.end());
    }

    IteratorWrapper end() {
        return IteratorWrapper(collection.end(), collection.end());
    }
};

class Producer {
public:
    using StringCoefficients = std::tuple<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>>;
    using Data = std::vector<std::string>;
private:
    std::atomic<bool>& done_flag;

    Data& inputData;
    SortOfLockFreeQueue<StringCoefficients>& inputQueue;

public:
    uint32_t produced = 0;
    Producer(Data& inputData, SortOfLockFreeQueue<StringCoefficients>& inputQueue, std::atomic<bool>& df) : inputData(inputData), inputQueue(inputQueue), done_flag(df) {}
    
    void run() {
        TupleProducer<Data, 3> tp(inputData);
        for (auto it = tp.begin(); it != tp.end(); ++it) {
          inputQueue.enqueue(*it);
        }
        done_flag.store(true, std::memory_order_release);
    }
};

class Solver {
private:
    std::atomic<bool>& input_done;
    std::atomic<bool>& output_done;
    std::atomic<uint32_t> workersDone;

    SortOfLockFreeQueue<Producer::StringCoefficients>& inputQueue;
    SortOfLockFreeQueue<QuadraticEquationSolver::Solution>& outputQueue;

    void done() {
      uint32_t left = workersDone.fetch_sub(1);
      // fetch_sub return previous value, so 1 means we are the last worker
      if (left == 1) {
        output_done.store(true, std::memory_order_release);
      }
    }

public:
    Solver(SortOfLockFreeQueue<Producer::StringCoefficients>& inputQueue, SortOfLockFreeQueue<QuadraticEquationSolver::Solution>& outputQueue, std::atomic<bool>& idf, std::atomic<bool>& odf, uint32_t workers) : inputQueue(inputQueue), outputQueue(outputQueue), input_done(idf), output_done(odf), workersDone(workers) {}
    
    void run() {
        Producer::StringCoefficients task;
        QuadraticEquationSolver eq;
        while (true) {
          while (inputQueue.tryDequeue(task)) {
              outputQueue.enqueue(eq.solve(task));
          }
          std::this_thread::yield();
          if (inputQueue.is_empty() && input_done.load(std::memory_order_acquire)) {
            break;
          }
        }
        done();
    }
};

class Printer {
private:
    std::atomic<bool>& done_flag;

    std::ostream& output;
    SortOfLockFreeQueue<QuadraticEquationSolver::Solution>& outputQueue;

public:
    Printer(std::ostream& output, SortOfLockFreeQueue<QuadraticEquationSolver::Solution>& outputQueue, std::atomic<bool>& df) : output(output), outputQueue(outputQueue), done_flag(df) {}

    void run() {
        QuadraticEquationSolver::Solution task;
        while (true) {
            while (outputQueue.tryDequeue(task)) {
                output << task << std::endl;
            }
            std::this_thread::yield();
            if (outputQueue.is_empty() && done_flag.load(std::memory_order_acquire)) {
              break;
            }
        }
    }
};

class Application {
private:
    static const unsigned int MEMORY_SIZE = 1024;


    std::ostream& output;
    Producer::Data& inputData;
    SortOfLockFreeQueue<Producer::StringCoefficients> inputQueue;
    SortOfLockFreeQueue<QuadraticEquationSolver::Solution> outputQueue;

public:
    Application(Producer::Data& data, std::ostream& out) : inputData(data), output(out),
              inputQueue(MEMORY_SIZE), outputQueue(MEMORY_SIZE) {}

    void run() {
        int32_t solversCount = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 2);
        std::atomic<bool> input_flag = false, output_flag = false;
        std::atomic<uint8_t> output_synchro_ctr = solversCount;

        Producer producer(inputData, inputQueue, input_flag);
        Printer printer(output, outputQueue, output_flag);

        Solver solver(inputQueue, outputQueue, input_flag, output_flag, solversCount);
        std::vector<std::thread> solvers(solversCount);

        auto createSolverThread = [&] () { return std::thread([&] { solver.run(); }); };
        std::thread producerThread([&] { producer.run(); });
        std::generate_n(solvers.begin(), solversCount, createSolverThread);
        std::thread printerThread([&] { printer.run(); });

        producerThread.join();
        std::for_each(solvers.begin(), solvers.end(), [] (std::thread& t) {t.join();}); 
        printerThread.join();
        assert(inputQueue.is_empty());
        assert(outputQueue.is_empty());
    }
};

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments(argv+1, argv + argc);
    Application app(arguments, std::cout);
    app.run();
    std::cout.flush();
    return 0;
}
