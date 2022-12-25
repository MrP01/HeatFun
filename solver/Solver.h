#pragma once

#include "TschebFun.h"
#include <muparserx/mpParser.h>

class HeatSolver {
 protected:
  TschebFun currentU = TschebFun(1);

 public:
  HeatSolver();
  void setup(Vector u0);
  Vector evaluateExpression(std::string expression, Vector x);
};
