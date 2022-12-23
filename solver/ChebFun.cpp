#include "ChebFun.h"

ChebFun::ChebFun(xt::xarray<double> coeffs) : coefficients(coeffs) {}

ChebFun ChebFun::interpolantThrough(xt::xarray<double> y) { return ChebFun(y); }
