#pragma once
#include "CompileTime.h"

#define RAND_SEED                                         \
  ((__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 +   \
   (__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 + \
   (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000)

constexpr int LinearCongruentGenerator(int rounds) {
  return 1013904223 +
         1664525 * ((rounds > 0) ? LinearCongruentGenerator(rounds - 1)
                                 : RAND_SEED & 0xFFFFFFFF);
}

#define RAND() \
  EnsureCompileTime<LinearCongruentGenerator(10)>::value  // 10 Rounds
#define RAND_RANGE(min, max) (min + (RAND() % (max - min + 1)))
static constexpr char XOR_KEY = (char)RAND_RANGE(0, 0xff);

template <int... Pack>
struct IndexList {};

template <typename IndexList, int Right>
struct Append;
template <int... Left, int Right>
struct Append<IndexList<Left...>, Right> {
  typedef IndexList<Left..., Right> Result;
};

template <int N>
struct ConstructIndexList {
  typedef typename Append<typename ConstructIndexList<N - 1>::Result,
                          N - 1>::Result Result;
};
template <>
struct ConstructIndexList<0> {
  typedef IndexList<> Result;
};

constexpr char EncryptCharacter(const char Character, int Index) {
  return Character ^ (XOR_KEY + Index);
}

template <typename IndexList>
class CXorString;
template <int... Index>
class CXorString<IndexList<Index...> > {
 private:
  char Value[sizeof...(Index) + 1];

 public:
  constexpr CXorString(const char* const String)
      : Value{EncryptCharacter(String[Index], Index)...} {}

  char* decrypt() {
    for (int t = 0; t < sizeof...(Index); t++) {
      Value[t] = Value[t] ^ (XOR_KEY + t);
    }
    Value[sizeof...(Index)] = '\0';
    return Value;
  }

  char* get() { return Value; }
};
#define XorS(X, String) \
  CXorString<ConstructIndexList<sizeof(String) - 1>::Result> X(String)
