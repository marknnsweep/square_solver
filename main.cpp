#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
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
        bool operator==(const Iterator& other) const {
            return current == other.current;
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
            --current;

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

class Application {
private:
    std::ostream& output;
    InputData& inputData;

public:
    Application(InputData& data, std::ostream& out) : inputData(data), output(out) {}

    void run() {
        for (auto it = inputData.begin(); it != inputData.end(); ++it) {
            const auto& [a, b, c] = *it;
            QuadraticEquation<int, double> task(a, b, c);
            output << task.solve() << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    InputData data(argv, argc);
    Application app(data, std::cout);
    app.run();
    return 0;
}