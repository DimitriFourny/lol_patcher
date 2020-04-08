#pragma once

template <int X>
struct EnsureCompileTime {
  enum : int { value = X };
};

template <unsigned int X>
struct EnsureCompileTimeU {
  enum : unsigned int { value = X };
};
