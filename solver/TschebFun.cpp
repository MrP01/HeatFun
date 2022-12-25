#include "TschebFun.h"

static const double pi = xt::numeric_constants<double>::PI;

TschebFun::TschebFun(Vector coeffs) : coefficients(coeffs) {}

Vector TschebFun::chebpoints(size_t N) { return -xt::cos(xt::linspace(0.0, pi, N)); }

TschebFun TschebFun::interpolantThrough(Vector y) {
  int order = y.size();
  auto j = xt::linspace(0.0, (double)order, order) * (pi / order);
  Vector coeffs = xt::zeros_like(y); // as many coefficients as data points
  coeffs[0] = xt::sum(y)() / order;
  for (size_t k = 1; k < order; k++)
    coeffs[k] = (2.0 / order) * xt::sum(y * xt::cos(j * k))();
  assert(coeffs.size() == order);

  std::cout << "Chebpoints: " << std::endl;
  Vector chebpts = TschebFun::chebpoints(order);
  std::copy(chebpts.begin(), chebpts.end(), std::ostream_iterator<double>(std::cout, ", "));
  std::cout << std::endl;
  std::cout << "Constructed interpolant through " << std::endl;
  std::copy(y.begin(), y.end(), std::ostream_iterator<float>(std::cout, ", "));
  std::cout << std::endl;
  std::cout << "Resulting Chebyshev coefficients: " << std::endl;
  std::copy(coeffs.begin(), coeffs.end(), std::ostream_iterator<float>(std::cout, ", "));
  std::cout << std::endl;
  return TschebFun(coeffs);
}

Vector TschebFun::evaluateOn(Vector x) {
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
