#pragma once

template <int X>
struct EnsureCompileTime {
  enum : int { value = X };
};
