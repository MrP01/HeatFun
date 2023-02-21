#include "Solver.h"
#include <xtensor/xadapt.hpp>

HeatSolver::HeatSolver() {}

void HeatSolver::setup(Vector u0) {
  currentU = TschebFun::interpolantThrough(u0);
  size_t N = currentU.order();
  tau_1 = (2 + (N - 2) * (N - 4)) * ((N % 2 == 1) ? 1.0 : -1.0);
  tau_2 = (2 + (N - 1) * (N - 3)) * ((N % 2 == 0) ? 1.0 : -1.0);

  Vector boundary_values = currentU.evaluateOn({-1.0, 1.0});
  left_bc.type = Dirichlet;
  left_bc.value = boundary_values[0];
  right_bc.type = Dirichlet;
  right_bc.value = boundary_values[1];
}

void HeatSolver::iterate() {
  // base step, leaves two degrees of freedom a_{N}, a_{N-1}
  TschebFun previousU = currentU;
  currentU = previousU + previousU.derivative().derivative() * (dt * alpha);
  forceBoundaryConditions(&currentU, left_bc, right_bc);

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

void HeatSolver::forceBoundaryConditions(TschebFun *series, BC left, BC right) {
  // force boundary conditions by setting a_{N}, a_{N-1}
  size_t degree = series->degree();
  assert(degree >= 6);
  assert(right.type == Dirichlet && "Only Dirichlet BC supported on the right-hand side.");
  Vector fixed_coefficients = xt::view(series->coefficients, xt::range(0, degree - 2));
  double sigma_2 = xt::sum(fixed_coefficients)();
  switch (left.type) {
    case Dirichlet: {
      double sigma_1 = xt::sum(xt::pow((double)-1, xt::arange(degree - 2)) * fixed_coefficients)();
      if (degree % 2 == 1) { // odd degree
        series->coefficients[degree - 1] = (left.value + right.value - sigma_1 - sigma_2) / 2.0;
        series->coefficients[degree] = right.value - series->coefficients[degree - 1] - sigma_2;
      } else { // even degree
        series->coefficients[degree] = (left.value + right.value - sigma_1 - sigma_2) / 2.0;
        series->coefficients[degree - 1] = right.value - series->coefficients[degree] - sigma_2;
      }
      break;
    }
    case Neumann: {
      size_t N = series->order();
      Vector K = xt::arange<double>(3, degree - 2);
      Vector middle_coefficients = xt::view(series->coefficients, xt::range(3, degree - 2));
      double sigma_3 = series->coefficients[1] - 4 * series->coefficients[2] +
                       xt::sum((2 + K * (K - 2)) * xt::pow((double)-1, K - 1) * middle_coefficients)();
      std::cout << tau_1 << ", " << tau_2 << ", " << sigma_2 << ", " << sigma_3 << std::endl;
      series->coefficients[degree] = (left.value + tau_1 * sigma_2 - sigma_3 - tau_1 * right.value) / (tau_2 - tau_1);
      series->coefficients[degree - 1] = right.value - sigma_2 - series->coefficients[degree];
      std::cout << "Set highest-order coefficients to: a_{N-2} = " << series->coefficients[degree - 1]
                << " and a_{N-1} = " << series->coefficients[degree] << std::endl;
      std::cout << "Derivative at x=-1: " << series->derivative().evaluateOn({-1})[0] << ", should be: " << left.value
                << std::endl;
      break;
    }
  }
  // Vector boundary_values = series->evaluateOn({-1.0, 1.0});
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
