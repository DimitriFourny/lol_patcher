#pragma once
#include <stddef.h>

class Math {
 public:
  static double Fact(int n);
  static double Pow(double x, size_t y);
  static double IntDist(double a, double b);
  static double Cos(double x);
  static float Cos(float x);
  static double Sin(double x);
  static float Sin(float x);
};