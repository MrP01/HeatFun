#pragma once

#include <xtensor/xarray.hpp>

typedef xt::xarray<double> Vector;

class TschebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  TschebFun(Vector coeffs);
  size_t order() { return coefficients.size(); };
  size_t degree() { return coefficients.size() - 1; };
  Vector evaluateOn(Vector x);
  static TschebFun interpolantThrough(Vector y);
  static Vector chebpoints(size_t N);
};
