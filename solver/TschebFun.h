#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>

typedef xt::xarray<double> Vector;

class TschebFun {
 public:
  xt::xarray<double> coefficients;

 public:
  TschebFun(Vector coeffs);
  size_t order() { return coefficients.size(); };
  size_t degree() { return coefficients.size() - 1; };
  Vector evaluateOn(Vector x);
  TschebFun derivative();
  static TschebFun interpolantThrough(Vector y);

  static Vector equipoints(size_t N);
  static Vector chebpoints(size_t N);
  static Vector modifiedEquipoints(size_t N);
  static Vector modifiedChebpoints(size_t N);

  TschebFun operator+(const TschebFun &other);
  TschebFun operator+(const double &addend);
  TschebFun operator-(const TschebFun &other);
  TschebFun operator*(const double &factor);
  TschebFun operator-();

  Vector evaluateOnInterval(Vector x, double left, double right);
};

void print(std::string text, Vector x);
