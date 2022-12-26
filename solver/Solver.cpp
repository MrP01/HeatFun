#include "Solver.h"
#include <xtensor/xadapt.hpp>

HeatSolver::HeatSolver() {}

void HeatSolver::setup(Vector u0) { currentU = TschebFun::interpolantThrough(u0); }

void HeatSolver::iterate() { currentU = currentU - currentU.derivative().derivative() * dt * alpha; }

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
