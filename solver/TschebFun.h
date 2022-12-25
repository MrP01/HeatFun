#pragma once

#include <xtensor/xarray.hpp>

typedef xt::xarray<double> Vector;

class TschebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  TschebFun(Vector coeffs);
  size_t degree() { return coefficients.size(); };
  Vector evaluateOn(Vector x);
  static TschebFun interpolantThrough(Vector y);
  static Vector chebpoints(size_t N);
};
