#pragma once

#include <xtensor/xarray.hpp>

typedef xt::xarray<double> Vector;

class ChebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  ChebFun(Vector coeffs);
  size_t degree() { return coefficients.size(); };
  Vector evaluateOn(Vector x);
  static ChebFun interpolantThrough(Vector y);
  static Vector chebpoints(size_t N);
};
