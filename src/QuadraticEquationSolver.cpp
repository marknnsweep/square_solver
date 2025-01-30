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