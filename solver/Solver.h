#pragma once

#include "TschebFun.h"
#include <muparserx/mpParser.h>

struct Optimisations {
  bool adaptiveDt = false;
  double adaptiveGoalDu = 1e-2;
  bool linearMultistep = false;
};

enum BoundaryConditionType { Dirichlet, Neumann };
struct BoundaryCondition {
  enum BoundaryConditionType type = Dirichlet;
  double value = 0;
};
typedef struct BoundaryCondition BC;

class HeatSolver {
 public:
  double alpha = 1.0;
  double dt = 1e-5;
  struct BoundaryCondition left_bc;
  struct BoundaryCondition right_bc;
  double totalTime = 0;
  TschebFun currentU = TschebFun(1);

  struct Optimisations optimisations;
  double tau_1 = 0;
  double tau_2 = 0;

 public:
  HeatSolver();
  virtual void setup(Vector u0);
  virtual void iterate();
  virtual void forceBoundaryConditions(TschebFun *series, BC left, BC right);
};

Vector evaluateExpression(std::string expression, Vector x);
