#include "Math.h"
#define M_PI 3.141592654f

// static
double Math::Fact(int n) {
  if (n == 1 || n == 0) {
    return 1;
  }

  double result = 1;
  for (int i = n; i > 1; i--) {
    result *= i;
  }

  return result;
}

// static
double Math::Pow(double x, size_t y) {
  if (y == 0) {
    return 1;
  }
  double r = x;
  for (size_t i = 0; i < y - 1; i++) {
    r *= x;
  }
  return r;
}

// static
double Math::IntDist(double a, double b) {
  if (a > b) {
    return a - b;
  } else {
    return b - a;
  }
}

// static
double Math::Cos(double x) {
  if (x > 0) {
    while (x >= 2 * M_PI) {
      x -= 2 * M_PI;
    }
  } else if (x < 0) {
    while (x <= -2 * M_PI) {
      x += 2 * M_PI;
    }
  }
  double y = 1;
  bool neg = true;
  double last_value;
  bool first_loop = true;
  unsigned int n = 1;
  while (true) {
    if (neg) {
      y -= Pow(x, n * 2) / Fact(n * 2);
      neg = false;
    } else {
      y += Pow(x, n * 2) / Fact(n * 2);
      neg = true;
    }

    if (!first_loop) {
      if (IntDist(y, last_value) < 0.00001) {
        break;
      }
    } else {
      first_loop = false;
    }
    last_value = y;
    n++;
  }

  return y;
}

// static
float Math::Cos(float x) {
  return static_cast<float>(Cos(static_cast<double>(x)));
}

// static
double Math::Sin(double x) {
  if (x > 0) {
    while (x >= 2 * M_PI) {
      x -= 2 * M_PI;
    }
  } else if (x < 0) {
    while (x <= -2 * M_PI) {
      x += 2 * M_PI;
    }
  }
  double y = x;
  bool neg = true;
  double last_value;
  bool first_loop = true;
  unsigned int n = 1;

  while (true) {
    if (neg) {
      y -= Pow(x, 2 * n + 1) / Fact(2 * n + 1);
      neg = false;
    } else {
      y += Pow(x, 2 * n + 1) / Fact(2 * n + 1);
      neg = true;
    }

    if (!first_loop) {
      if (IntDist(y, last_value) < 0.00001) {
        break;
      }
    } else {
      first_loop = false;
    }
    last_value = y;
    n++;
  }

  return y;
}

// static
float Math::Sin(float x) {
  return static_cast<float>(Sin(static_cast<double>(x)));
}
