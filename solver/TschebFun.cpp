#include "TschebFun.h"

#define max(a, b) (a > b) ? a : b
static const double pi = xt::numeric_constants<double>::PI;
static const double epsilon = 1e-9;

void print(std::string text, Vector x) {
  // #ifndef SILENT
  std::cout << text << ": ";
  std::copy(x.begin(), x.end(), std::ostream_iterator<float>(std::cout, ", "));
  std::cout << std::endl;
  // #endif
}

TschebFun::TschebFun(Vector coeffs) : coefficients(coeffs) {
  // xt::filter(coefficients, xt::abs(coefficients) < epsilon) = 0;
  // print("Constructed TschebFun with", coefficients);
}

Vector TschebFun::equipoints(size_t N) { return xt::linspace(0.0, pi, N); }
Vector TschebFun::chebpoints(size_t N) { return xt::cos(equipoints(N)); }
Vector TschebFun::modifiedEquipoints(size_t N) { return (xt::arange<double>(0, N) + 0.5) * (pi / N); }
Vector TschebFun::modifiedChebpoints(size_t N) { return xt::cos(modifiedEquipoints(N)); }

TschebFun TschebFun::interpolantThrough(Vector y) {
  int order = y.size(), degree = order - 1;
  Vector j = modifiedEquipoints(order);
  Vector coeffs = xt::zeros_like(y); // as many coefficients as data points
  coeffs[0] = xt::sum(y)() / order;
  for (size_t k = 1; k < order; k++)
    coeffs[k] = (2.0 / order) * xt::sum(y * xt::cos(j * k))();
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

TschebFun TschebFun::derivative() {
  int n = coefficients.size();
  n = n - 1;
  Vector coeffs = coefficients; // make a copy
  Vector derivative = xt::zeros<double>({n});
  for (size_t j = n; j > 2; j--) {
    derivative[j - 1] = (2 * j) * coeffs[j];
    coeffs[j - 2] += (j * coeffs[j]) / (j - 2);
  }
  if (n > 1)
    derivative[1] = 4 * coeffs[2];
  derivative[0] = coeffs[1];
  return TschebFun(derivative);
}

TschebFun TschebFun::operator+(const TschebFun &other) {
  size_t mine = coefficients.size(), theirs = other.coefficients.size();
  Vector new_coeffs = xt::zeros<double>({max(mine, theirs)});
  xt::view(new_coeffs, xt::range(0, coefficients.size())) += coefficients;
  xt::view(new_coeffs, xt::range(0, other.coefficients.size())) += other.coefficients;
  return TschebFun(new_coeffs);
}
TschebFun TschebFun::operator+(const double &addend) {
  TschebFun result = TschebFun(*this);
  result.coefficients[0] += addend;
  return result;
}
TschebFun TschebFun::operator-(const TschebFun &other) {
  size_t mine = coefficients.size(), theirs = other.coefficients.size();
  Vector new_coeffs = xt::zeros<double>({max(mine, theirs)});
  xt::view(new_coeffs, xt::range(0, coefficients.size())) -= coefficients;
  xt::view(new_coeffs, xt::range(0, other.coefficients.size())) -= other.coefficients;
  return TschebFun(new_coeffs);
}
TschebFun TschebFun::operator*(const double &factor) { return TschebFun(coefficients * factor); }

TschebFun TschebFun::operator-() { return TschebFun(-coefficients); }

Vector TschebFun::evaluateOnInterval(Vector x, double left, double right) {
  double beta_1 = 2 / (right - left);
  double beta_2 = left - 1;
  return TschebFun::evaluateOn(beta_1 * x + beta_2);
}
