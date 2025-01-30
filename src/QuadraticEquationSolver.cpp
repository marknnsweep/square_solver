#include <cmath>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>

#include "QuadraticEquationSolver.hpp"

QuadraticEquationSolver::Solution
QuadraticEquationSolver::solve(const Data &stringCoeff) const {
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
      if (c == 0) {
        // any value will be a solution, no xmin
        return Solution(coefficients, Solution::AllRoots());
      }
      // there is no a value that is a solution, no xmin
      return Solution(coefficients, Solution::NoRoots());
    } else {
      double root = -static_cast<double>(c) / b;
      // single root, no xmin
      return Solution(coefficients, Solution::Root(root));
    }
  }

  double sum = -static_cast<double>(b) / a;
  double product = static_cast<double>(c) / a;

  double discriminant = sum * sum - 4 * product;
  if (discriminant < 0) {
    // no roots, xmin exists
    return Solution(coefficients, Solution::Xmin(-sum / 2),
                    Solution::NoRoots());
  }

  double sqrt_d = std::sqrt(discriminant);
  double root1 = (sum + sqrt_d) / 2;
  double root2 = (sum - sqrt_d) / 2;

  double xmin = -sum / 2;

  return Solution(coefficients, root1, root2, xmin);
}

std::ostream &operator<<(std::ostream &os,
                         const QuadraticEquationSolver::Solution &solution) {
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
    if (solution.all_roots) {
      os << "All roots";
    } else if (!solution.root1 && !solution.root2) {
      os << "No roots";
    } else {
      os << "(";
      os << (solution.root1.has_value() ? std::to_string(solution.root1.value())
                                        : "");
      os << (solution.root2.has_value()
                 ? ", " + std::to_string(solution.root2.value())
                 : "");
      os << ")";
    }
    os << (solution.xmin.has_value()
               ? " Xmin=" + std::to_string(solution.xmin.value())
               : " No Xmin");
  }

  return os;
}

std::optional<int64_t>
QuadraticEquationSolver::parseInteger(const std::string &s) {
  try {
    std::size_t pos = 0;
    int64_t value = std::stoll(s, &pos); // parse numeric part
    if (pos != s.size()) {
      // Some non-numeric characters remain => fail
      throw std::invalid_argument("Invalid numeric string");
    }
    return value;
  } catch (...) {
    return std::nullopt;
  }
}

QuadraticEquationSolver::Coefficients
QuadraticEquationSolver::parseCoefficietns(const Data &stringCoefficients) {
  const auto &[a_str, b_str, c_str] = stringCoefficients;
  return std::make_tuple(a_str ? parseInteger(*a_str) : std::nullopt,
                         b_str ? parseInteger(*b_str) : std::nullopt,
                         c_str ? parseInteger(*c_str) : std::nullopt);
}