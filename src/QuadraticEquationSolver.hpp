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
  static std::optional<int64_t> parseInteger(const std::string &s);

public:
  struct Solution {
    struct Xmin {
      double v;
      explicit Xmin(double v) : v(v) {}
    };
    struct Root {
      double v;
      explicit Root(double v) : v(v) {}
    };
    struct AllRoots {};

    Solution() = default;

    Solution(Coefficients coeff) : coefficients(coeff) {};

    Solution(Coefficients coeff, bool incorrect_input)
        : coefficients(coeff), incorrect_input(incorrect_input){};
    Solution(Coefficients coeff, AllRoots)
        : coefficients(coeff), all_roots(true){};

    Solution(Coefficients coeff, Xmin xmin)
        : coefficients(coeff), xmin(xmin.v){};
    Solution(Coefficients coeff, Root root)
        : coefficients(coeff), root1(root.v){};

    Solution(Coefficients coeff, double xmin, double root)
        : coefficients(coeff), xmin(xmin), root1(root) {}
    Solution(Coefficients coeff, double root1, double root2, double xmin)
        : coefficients(coeff), xmin(xmin), root1(root1), root2(root2) {}

    Coefficients coefficients;
    std::optional<double> root1;
    std::optional<double> root2;
    std::optional<double> xmin;
    bool all_roots = false;
    bool incorrect_input = false;
  };

  QuadraticEquationSolver() = default;

  Solution solve(const Data &stringCoeff) const;

  friend std::ostream &operator<<(std::ostream &os, const Solution &solution);

private:
  static Coefficients parseCoefficietns(const Data &stringCoefficients);
};

#endif // QUADRATIC_EQUATION_SOLVER_HPP