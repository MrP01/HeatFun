#include <xtensor/xarray.hpp>

class ChebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  ChebFun(xt::xarray<double> coeffs);
  static ChebFun interpolantThrough(xt::xarray<double> y);
};
