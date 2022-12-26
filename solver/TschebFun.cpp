#include "TschebFun.h"

static const double pi = xt::numeric_constants<double>::PI;

void print(std::string text, Vector x) {
  std::cout << text << ": ";
  std::copy(x.begin(), x.end(), std::ostream_iterator<float>(std::cout, ", "));
  std::cout << std::endl;
}

TschebFun::TschebFun(Vector coeffs) : coefficients(coeffs) {}

Vector TschebFun::chebpoints(size_t N) { return -xt::cos(xt::linspace(0.0, pi, N)); }
Vector TschebFun::modifiedChebpoints(size_t N) {
  return xt::sin(pi / N / 2.0 * xt::arange<double>(-(int)N + 1, N + 1, 2));
}

TschebFun TschebFun::interpolantThrough(Vector y) {
  int order = y.size(), degree = order - 1;
  Vector j = (xt::linspace((double)degree, 0.0, order) + 0.5) * (pi / order);
  Vector coeffs = xt::zeros_like(y); // as many coefficients as data points
  coeffs[0] = xt::sum(y)() / order;
  for (size_t k = 1; k < order; k++)
    coeffs[k] = (2.0 / order) * xt::sum(y * xt::cos(j * k))();
  assert(coeffs.size() == order);
  print("Resulting Chebyshev coefficients", coeffs);
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
  }
  return (U_k - U_kp2 + coefficients[0]) / 2.0;
}
