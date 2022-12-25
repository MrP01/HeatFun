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
  Vector U_kp2;
  Vector U_kp1 = xt::zeros_like(x);
  Vector U_k = xt::ones_like(x) * coefficients[coefficients.size() - 1];
  for (int k = coefficients.size() - 2; k >= 0; k--) {
    U_kp2 = U_kp1;
    U_kp1 = U_k;
    U_k = 2.0 * x * U_kp1 - U_kp2 + coefficients[k];
    std::cout << k << " U_k[0] = " << U_k[0] << std::endl;
  }
  return (U_k - U_kp2 + coefficients[0]) / 2.0;
}
