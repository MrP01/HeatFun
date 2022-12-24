#pragma once

#include "ChebFun.h"
#include <muparserx/mpParser.h>

class HeatSolver {
 public:
  HeatSolver();
  void setup(Vector u0);
  Vector evaluateExpression(std::string expression, Vector x);
};
