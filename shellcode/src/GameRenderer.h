#pragma once
#include <d3dx9.h>
#include <windows.h>

#define DEFINE_SETTERS(type, name, offset) \
  type Get##name() { return *reinterpret_cast<type*>(this + offset); }
#define DEFINE_SETTERS_PTR(type, name, offset) \
  type* Get##name() { return reinterpret_cast<type*>(this + offset); }

class GameRenderer {
 public:
  DEFINE_SETTERS(DWORD, ScreenWidth, 0x14)
  DEFINE_SETTERS(DWORD, ScreenHeight, 0x18)
  DEFINE_SETTERS_PTR(D3DXMATRIX, ViewMatrix, 0x68)
  DEFINE_SETTERS_PTR(D3DXMATRIX, ProjectionMatrix, 0xA8)
};

#undef DEFINE_SETTERS
#undef DEFINE_SETTERS_PTR