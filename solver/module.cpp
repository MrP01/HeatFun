#include <pybind11/pybind11.h>

int add(int i, int j) { return i + j; }

PYBIND11_MODULE(heatfun, m) {
  m.doc() = "HeatFun - taking care of your heat equation solver needs";
  m.def("add", &add, "A function that adds two numbers");
}
