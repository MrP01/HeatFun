#include <pybind11/pybind11.h>

int add(int i, int j) { return i + j; }

double *chebyshev_coefficients(double a, double b, int n, double f(double x)) {
  double angle;
  double *c;
  double *fx;
  int i;
  int j;
  double pi = 3.141592653589793;
  double x;

  fx = new double[n];

  for (i = 0; i < n; i++) {
    angle = (double)(2 * i + 1) * pi / (double)(2 * n);
    x = cos(angle);
    x = 0.5 * (a + b) + x * 0.5 * (b - a);
    fx[i] = f(x);
  }

  c = new double[n];

  for (i = 0; i < n; i++) {
    c[i] = 0.0;
    for (j = 0; j < n; j++) {
      angle = (double)(i * (2 * j + 1)) * pi / (double)(2 * n);
      c[i] = c[i] + fx[j] * cos(angle);
    }
  }

  for (i = 0; i < n; i++) {
    c[i] = 2.0 * c[i] / (double)(n);
  }

  delete[] fx;

  return c;
}

PYBIND11_MODULE(heatfun, m) {
  m.doc() = "HeatFun - taking care of your heat equation solver needs";
  m.def("add", &add, "A function that adds two numbers");
}
