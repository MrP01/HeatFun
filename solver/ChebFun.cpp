#include "ChebFun.h"
#include <cmath>

ChebFun::ChebFun(Vector coeffs) : coefficients(coeffs) {}

ChebFun ChebFun::interpolantThrough(Vector y) { return ChebFun(y); }

Vector ChebFun::chebpoints(size_t N) {
  return xt::cos(xt::linspace(0.0, N * xt::numeric_constants<double>::PI / (N - 1), N));
}
