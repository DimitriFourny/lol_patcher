#pragma once
#include <cstdint>
#include "CompileTime.h"

#define DJB_HASH(str) EnsureCompileTime<Hash::Djb(str)>::value

class Hash {
 public:
  static constexpr uint32_t Djb(const char* str) {
    uint32_t hash = 5381;
    for (size_t i = 0; str[i]; i++) {
      hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
  }
};
