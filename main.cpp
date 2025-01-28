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

#if defined(__GNUC__) || defined(__clang__)
    #define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define UNREACHABLE() __assume(false)
#else
    #define UNREACHABLE() std::abort()
#endif

template<typename IntType, typename FloatType>
class QuadraticEquation {
public:
    using Coefficients = std::tuple<std::optional<IntType>, std::optional<IntType>, std::optional<IntType>>;
    using Data = std::tuple<std::optional<std::string>, std::optional<std::string>, std::optional<std::string>>;
private:
    Coefficients coefficients;

    static std::optional<int> parseInteger(const std::string& s) {
        try {
            return std::stoi(s);
        } catch (...) {
            return std::nullopt;
        }
    }

public:

    struct Solution {
        Coefficients coefficients;
        std::optional<FloatType> root1;
        std::optional<FloatType> root2;
        std::optional<FloatType> xmin;
        bool no_roots = false;
        bool no_real_roots = false;
        bool no_xmin = false;
        bool incorrect_input = false;
    };

    // Constructor that converts vector of string tuples to Coefficients
    QuadraticEquation(const Data& stringCoefficients) {
        const auto& [a_str, b_str, c_str] = stringCoefficients;
        coefficients = std::make_tuple(
            a_str ? parseInteger(*a_str) : std::nullopt,
            b_str ? parseInteger(*b_str) : std::nullopt,
            c_str ? parseInteger(*c_str) : std::nullopt
            );
    }

    Solution solve() const {
        Solution solution;
        solution.coefficients = coefficients;

        // Check if all coefficients are valid
        if (!std::get<0>(coefficients).has_value() ||
            !std::get<1>(coefficients).has_value() ||
            !std::get<2>(coefficients).has_value()) {
            solution.incorrect_input = true;
            return solution;
        }

        IntType a = std::get<0>(coefficients).value();
        IntType b = std::get<1>(coefficients).value();
        IntType c = std::get<2>(coefficients).value();

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

        FloatType sum = -static_cast<FloatType>(b) / a;
        FloatType product = static_cast<FloatType>(c) / a;

        FloatType discriminant = sum * sum - 4 * product;
        if (discriminant < 0) {
            solution.no_roots = true;
            solution.no_real_roots = true;
            solution.xmin = -sum / 2;
            return solution;
        }

        FloatType sqrt_d = std::sqrt(discriminant);
        solution.root1 = (sum + sqrt_d) / 2;
        solution.root2 = (sum - sqrt_d) / 2;
        solution.xmin = -sum / 2;

        return solution;
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
};

template <typename T>
class CQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    bool done = false;

public:
    CQueue() = default;
    ~CQueue() = default;

    // Add an item to the queue
    void enqueue(const T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(item);
        cv.notify_one();
    }

    void enqueue(T&& item) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(item));
        cv.notify_one();
    }

    // Emplace an item directly in the queue
    template <typename... Args>
    void emplace(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.emplace(std::forward<Args>(args)...);
        cv.notify_one();
    }

    // Remove and return the front item from the queue
    bool dequeue(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty() || isDone(); });
        if (queue.empty()) {
            return false;
        }
        item = std::move(queue.front());
        queue.pop();
        return true;
    }

    // Mark the queue as done
    void setDone() {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
        cv.notify_all();
    }

    // Check if the queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    // Get the size of the queue
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }

    // Check if the queue is done
    bool isDone() const {
        std::lock_guard<std::mutex> lock(mtx);
        return done;
    }
};

class Printer {
private:
    std::ostream& output;
    CQueue<QuadraticEquation<int, double>::Solution>& outputQueue;

public:
    Printer(std::ostream& output, CQueue<QuadraticEquation<int, double>::Solution>& outputQueue) : output(output), outputQueue(outputQueue) {}

    void run() {
        QuadraticEquation<int, double>::Solution task;
        while (outputQueue.dequeue(task)) {
            output << task << std::endl;
        }
    }
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

    //using TupleType = decltype(std::declval<std::optional<ValueType>>(), std::make_index_sequence<Size>{});

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
    Data& inputData;
    CQueue<StringCoefficients>& inputQueue;

public:
    Producer(Data& inputData, CQueue<StringCoefficients>& inputQueue) : inputData(inputData), inputQueue(inputQueue) {}
    
    void run() {
        TupleProducer<Data, 3> tp(inputData);
        for (auto it = tp.begin(); it != tp.end(); ++it) {
                inputQueue.emplace(*it);
            }
        inputQueue.setDone();
    }
};

class Solver {
private:
    CQueue<Producer::StringCoefficients>& inputQueue;
    CQueue<QuadraticEquation<int, double>::Solution>& outputQueue;

public:
    Solver(CQueue<Producer::StringCoefficients>& inputQueue, CQueue<QuadraticEquation<int, double>::Solution>& outputQueue) : inputQueue(inputQueue), outputQueue(outputQueue) {}
    
    void run() {
        Producer::StringCoefficients task;
        while (inputQueue.dequeue(task)) {
            QuadraticEquation<int, double> eq(task);
            outputQueue.emplace(eq.solve());
        }
        outputQueue.setDone();
    }
};

class Application {
private:
    std::ostream& output;
    Producer::Data& inputData;
    CQueue<Producer::StringCoefficients> inputQueue;
    CQueue<QuadraticEquation<int, double>::Solution> outputQueue;

public:
    Application(Producer::Data& data, std::ostream& out) : inputData(data), output(out) {}

    void run() {
        Producer producer(inputData, inputQueue);
        Solver solver(inputQueue, outputQueue);
        Printer printer(output, outputQueue);

        int32_t solversCount = std::max(1u, std::thread::hardware_concurrency() - 2);
        std::vector<std::thread> solvers(solversCount);

        auto createSolverThread = [&] () { return std::thread([&] { solver.run(); }); };
        std::generate_n(solvers.begin(), solversCount, createSolverThread);
        std::thread printerThread([&] { printer.run(); });
        std::thread producerThread([&] { producer.run(); });

        producerThread.join();
        printerThread.join();
        std::for_each(solvers.begin(), solvers.end(), [] (std::thread& t) {t.join();}); 
    }
};

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments(argv+1, argv + argc);
    Application app(arguments, std::cout);
    app.run();
    return 0;
}