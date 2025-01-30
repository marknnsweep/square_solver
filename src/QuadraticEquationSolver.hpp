#ifndef QUADRATIC_EQUATION_SOLVER_HPP
#define QUADRATIC_EQUATION_SOLVER_HPP

#include <optional>
#include <ostream>

class QuadraticEquationSolver {
public:
  using Coefficients =
      std::tuple<std::optional<int64_t>, std::optional<int64_t>,
                 std::optional<int64_t>>;
  using Data =
      std::tuple<std::optional<std::string>, std::optional<std::string>,
                 std::optional<std::string>>;

private:
  static std::optional<int64_t> parseInteger(const std::string &s) {
    try {
      return std::stoll(s);
    } catch (...) {
      return std::nullopt;
    }
  }

public:
  struct Solution {
    Solution() = default;

    Solution(Coefficients coeff) : coefficients(coeff), incorrect_input(true) {}
    Solution(Coefficients coeff, double _xmin)
        : coefficients(coeff), xmin(_xmin), no_roots(true) {}
    Solution(Coefficients coeff, double root, bool)
        : coefficients(coeff), root1(root), incorrect_input(true) {}
    Solution(Coefficients coeff, double _xmin, double root)
        : coefficients(coeff), xmin(_xmin), root1(root) {}
    Solution(Coefficients coeff, double _xmin, double _root1, double _root2)
        : coefficients(coeff), xmin(_xmin), root1(_root1), root2(_root2) {}

    Coefficients coefficients;
    std::optional<double> root1;
    std::optional<double> root2;
    std::optional<double> xmin;
    bool no_roots = false;
    bool no_xmin = false;
    bool incorrect_input = false;
  };

  QuadraticEquationSolver() = default;

  Solution solve(const Data &stringCoeff) const;

  friend std::ostream &operator<<(std::ostream &os, const Solution &solution) {
    const auto &[a, b, c] = solution.coefficients;

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
      os << (solution.root1.has_value() ? std::to_string(solution.root1.value())
                                        : "No root 1");
      os << ", ";
      os << (solution.root2.has_value() ? std::to_string(solution.root2.value())
                                        : "No root 2");
      os << (solution.xmin.has_value()
                 ? ") Xmin=" + std::to_string(solution.xmin.value())
                 : ") No Xmin");
    }

    return os;
  }

private:
  static Coefficients parseCoefficietns(const Data &stringCoefficients) {
    const auto &[a_str, b_str, c_str] = stringCoefficients;
    return std::make_tuple(a_str ? parseInteger(*a_str) : std::nullopt,
                           b_str ? parseInteger(*b_str) : std::nullopt,
                           c_str ? parseInteger(*c_str) : std::nullopt);
  }
};

#endif // QUADRATIC_EQUATION_SOLVER_HPP