#pragma once

#include "TschebFun.h"
#include <muparserx/mpParser.h>

class HeatSolver {
 protected:
  double alpha = 1.0;
  double dt = 1e-3;
  TschebFun currentU = TschebFun(1);

 public:
  HeatSolver();
  void setup(Vector u0);
  void iterate();
};

Vector evaluateExpression(std::string expression, Vector x);
