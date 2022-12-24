#include "ChebFun.h"
#include <cmath>

ChebFun::ChebFun(Vector coeffs) : coefficients(coeffs) {}

Vector ChebFun::evaluate(Vector x) {
  double di, dip1, dip2;
  size_t N = coefficients.size();
  Vector cf = xt::zeros_like(x);
  for (size_t j = 0; j < x.size(); j++) {
    dip1 = 0.0;
    di = 0.0;
    // y = (2.0 * x[j] - a - b) / (b - a);

    for (size_t i = N - 1; 1 <= i; i--) {
      dip2 = dip1;
      dip1 = di;
      di = 2.0 * x[j] * dip1 - dip2 + coefficients[i];
    }
    cf[j] = x[j] * di - dip1 + 0.5 * coefficients[0];
  }
  return cf;
}

ChebFun ChebFun::interpolantThrough(Vector y) {
  size_t N = y.size();
  Vector coeffs = xt::zeros_like(y); // as many coefficients as data points
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      double angle = (double)(i * (2 * j + 1)) * xt::numeric_constants<double>::PI / (double)(2 * N);
      coeffs[i] = coeffs[i] + y[j] * cos(angle);
    }
  }
  coeffs = 2.0 * coeffs / N;
  return ChebFun(coeffs);
}

Vector ChebFun::chebpoints(size_t N) {
  return -xt::cos(xt::linspace(0.0, N * xt::numeric_constants<double>::PI / (N - 1), N));
}
