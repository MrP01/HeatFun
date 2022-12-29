#pragma once

#include "TschebFun.h"
#include <muparserx/mpParser.h>

struct Optimisations {
  bool adaptiveDt = false;
  double adaptiveGoalDu = 1e-2;
  bool linearMultistep = false;
};

class HeatSolver {
 public:
  double alpha = 1.0; // TODO: can we acccept negative alpha?
  double dt = 1e-5;
  double left_bc = 0;
  double right_bc = 0;
  double totalTime = 0;
  TschebFun currentU = TschebFun(1);

  struct Optimisations optimisations;

 public:
  HeatSolver();
  void setup(Vector u0);
  void iterate();
  void forceBoundaryConditions();
};

Vector evaluateExpression(std::string expression, Vector x);
