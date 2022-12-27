#include "Solver.h"
#include <xtensor/xadapt.hpp>

HeatSolver::HeatSolver() {}

void HeatSolver::setup(Vector u0) {
  currentU = TschebFun::interpolantThrough(u0);
  Vector boundary_values = currentU.evaluateOn({-1.0, 1.0});
  left_bc = boundary_values[0];
  right_bc = boundary_values[1];
  std::cout << "Set left BC: " << left_bc << std::endl;
  std::cout << "Set right BC: " << right_bc << std::endl;
}

void HeatSolver::iterate() {
  // base step, leaves two degrees of freedom a_{N}, a_{N-1}
  currentU = currentU - currentU.derivative().derivative() * (dt * alpha);
  forceBoundaryConditions();
}

void HeatSolver::forceBoundaryConditions() {
  // force boundary conditions by setting a_{N}, a_{N-1}
  size_t degree = currentU.degree();
  Vector fixed_coefficients = xt::view(currentU.coefficients, xt::range(0, degree - 2));
  double sigma_1 = xt::sum(xt::pow((double)-1, xt::arange(degree - 2)) * fixed_coefficients)();
  double sigma_2 = xt::sum(fixed_coefficients)();
  // TODO: optimise sigma_1 + sigma_2 (terms will cancel)
  if (degree % 2 == 1) { // odd degree
    currentU.coefficients[degree - 1] = (left_bc + right_bc - sigma_1 - sigma_2) / 2.0;
    currentU.coefficients[degree] = right_bc - currentU.coefficients[degree - 1] - sigma_2;
  } else { // even degree
    currentU.coefficients[degree] = (left_bc + right_bc - sigma_1 - sigma_2) / 2.0;
    currentU.coefficients[degree - 1] = right_bc - currentU.coefficients[degree] - sigma_2;
  }
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
