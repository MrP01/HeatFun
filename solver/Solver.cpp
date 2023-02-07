#include "Solver.h"
#include <xtensor/xadapt.hpp>

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
static const double kappa_0 = 35;
static const double alph = 0.5;
static const double E_start = -0.01;
static const double E_0 = -0.01;
static const double t_rev = 0.01;

HeatSolver::HeatSolver() {}

void HeatSolver::setup(Vector u0) {
  currentU = TschebFun::interpolantThrough(u0);
  size_t N = currentU.order();
  tau_1 = (2 + (N - 2) * (N - 4)) * ((N % 2 == 1) ? 1.0 : -1.0);
  tau_2 = (2 + (N - 1) * (N - 3)) * ((N % 2 == 0) ? 1.0 : -1.0);

  Vector boundary_values = currentU.evaluateOn({-1.0, 1.0});
  left_bc.type = Neumann;
  left_bc.value = 0;
  right_bc.value = boundary_values[1];
  std::cout << "Set left BC: type " << left_bc.type << " value: " << left_bc.value << std::endl;
  std::cout << "Set right BC: type " << right_bc.type << " value: " << right_bc.value << std::endl;
}

void HeatSolver::iterate() {
  double a = max(0, min(1, currentU.evaluateOn({-1})[0])), b = 1 - a;
  double E = (totalTime <= t_rev) ? (E_start + totalTime) : E_start + t_rev - (totalTime - t_rev);
  left_bc.value = kappa_0 * (a * exp(200 * (1 - alph) * (E - E_0)) - b * exp(-200 * alph * (E - E_0)));
  std::cout << "Set left BC: type " << left_bc.type << " value: " << left_bc.value << std::endl;

  // base step, leaves two degrees of freedom a_{N}, a_{N-1}
  TschebFun previousU = currentU;
  currentU = previousU + previousU.derivative().derivative() * (dt * alpha);
  forceBoundaryConditions();

  if (optimisations.adaptiveDt) {
    double change = xt::sum(xt::abs(currentU.coefficients - previousU.coefficients))();
    dt *= pow(optimisations.adaptiveGoalDu / change, 0.1);
    std::cout << "set new dt: " << dt << std::endl;
    if (change > 2 * optimisations.adaptiveGoalDu && dt > 1e-16) {
      std::cout << "reset previous computation" << std::endl;
      currentU = previousU;
      iterate();
    }
  }
  totalTime += dt;
}

void HeatSolver::forceBoundaryConditions() {
  // force boundary conditions by setting a_{N}, a_{N-1}
  size_t degree = currentU.degree();
  assert(degree >= 6);
  assert(right_bc.type == Dirichlet && "Only Dirichlet BC supported on the right-hand side.");
  Vector fixed_coefficients = xt::view(currentU.coefficients, xt::range(0, degree - 2));
  double sigma_2 = xt::sum(fixed_coefficients)();
  switch (left_bc.type) {
    case Dirichlet: {
      double sigma_1 = xt::sum(xt::pow((double)-1, xt::arange(degree - 2)) * fixed_coefficients)();
      if (degree % 2 == 1) { // odd degree
        currentU.coefficients[degree - 1] = (left_bc.value + right_bc.value - sigma_1 - sigma_2) / 2.0;
        currentU.coefficients[degree] = right_bc.value - currentU.coefficients[degree - 1] - sigma_2;
      } else { // even degree
        currentU.coefficients[degree] = (left_bc.value + right_bc.value - sigma_1 - sigma_2) / 2.0;
        currentU.coefficients[degree - 1] = right_bc.value - currentU.coefficients[degree] - sigma_2;
      }
      break;
    }
    case Neumann: {
      size_t N = currentU.order();
      Vector K = xt::arange<double>(3, degree - 2);
      Vector middle_coefficients = xt::view(currentU.coefficients, xt::range(3, degree - 2));
      double sigma_3 = currentU.coefficients[1] - 4 * currentU.coefficients[2] +
                       xt::sum((2 + K * (K - 2)) * xt::pow((double)-1, K - 1) * middle_coefficients)();
      std::cout << tau_1 << ", " << tau_2 << ", " << sigma_2 << ", " << sigma_3 << std::endl;
      currentU.coefficients[degree] =
          (left_bc.value + tau_1 * sigma_2 - sigma_3 - tau_1 * right_bc.value) / (tau_2 - tau_1);
      currentU.coefficients[degree - 1] = right_bc.value - sigma_2 - currentU.coefficients[degree];
      std::cout << "Set highest-order coefficients to: a_{N-2} = " << currentU.coefficients[degree - 1]
                << " and a_{N-1} = " << currentU.coefficients[degree] << std::endl;
      std::cout << "Derivative at x=-1: " << currentU.derivative().evaluateOn({-1})[0]
                << ", should be: " << left_bc.value << std::endl;
      break;
    }
  }
  // Vector boundary_values = currentU.evaluateOn({-1.0, 1.0});
  // std::cout << "Value left: " << boundary_values[0] << " and value right: " << boundary_values[1] << std::endl;
}

Vector evaluateExpression(std::string expression, Vector x) {
  mup::Value variable;
  mup::ParserX parser;
  parser.DefineVar("x", mup::Variable(&variable));
  parser.SetExpr(expression);
  std::vector<double> y;
  for (auto iter = x.begin(); iter != x.end(); iter++) {
    variable = *iter;
    y.push_back(parser.Eval().GetFloat());
  }
  return xt::adapt(y, {y.size()});
}
