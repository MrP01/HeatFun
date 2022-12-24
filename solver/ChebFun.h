#pragma once

#include <xtensor/xarray.hpp>

typedef xt::xarray<double> Vector;

class ChebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  ChebFun(Vector coeffs);
  static ChebFun interpolantThrough(Vector y);
  static Vector chebpoints(size_t N);
};
