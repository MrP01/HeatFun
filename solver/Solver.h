#pragma once

#include "ChebFun.h"
#include <muparserx/mpParser.h>

class HeatSolver {
 protected:
  ChebFun currentU = ChebFun(1);

 public:
  HeatSolver();
  void setup(Vector u0);
  Vector evaluateExpression(std::string expression, Vector x);
};
