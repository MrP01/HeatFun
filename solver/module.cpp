#include "Solver.h"
#include "pybind11_xarray.hpp"
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <xtensor/xadapt.hpp>

namespace py = pybind11;

Vector solve(Vector u0, double T, Vector x) {
  HeatSolver solver{};
  solver.setup(u0);
  for (size_t i = 0; i <= (size_t)(T / solver.dt); i++)
    solver.iterate();
  return solver.currentU.evaluateOn(x);
}

PYBIND11_MODULE(heatfun, m) {
  m.doc() = "HeatFun - taking care of your heat equation solver needs";
  m.def("modifiedChebpoints", &TschebFun::modifiedChebpoints, "Returns the modified chebpoints required for sampling.");
  m.def("evaluateExpression", &evaluateExpression, "Evaluates the expression using the internal parser engine.");
  m.def("solve", &solve, "Solve the heat equation given initial condition u0(x) and time t.");
}
