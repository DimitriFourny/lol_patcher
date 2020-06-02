#pragma once

class GameString {
 public:
  union {
    char* strPtr;
    char strVal[16];
  };
  int len;
  int max;

  char* c_str() {
    if (max >= 16) {
      return strPtr;
    }
    return strVal;
  }
};