#pragma once
#include <d3dx9.h>
#include "Vector.h"

class GameColor {
 public:
  unsigned char b, g, r;

  GameColor() : r(0), g(0), b(0) {}
  GameColor(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

typedef void(__cdecl* type_world_to_screen)(Vector3* world, Vector3* screen);
typedef void(
    __cdecl* type_draw_circle)(Vector3* position, float range, GameColor* color, int a4, float a5, int a6, float alpha);

class Draw {
 public:
  static void Initialize(LPDIRECT3DDEVICE9 d3d9_device,
                         size_t screen_width,
                         size_t screen_height,
                         type_world_to_screen world_to_screen,
                         type_draw_circle draw_circle);
  static bool WorldToScreen(Vector3* pos, Vector2* pos2D);
  static void DrawLine(Vector2 start, Vector2 end, float width, bool antialias, D3DCOLOR color);
  static void DrawCircle3D(Vector3 pos, float radius, GameColor color);
  static bool DrawText2D(Vector2 pos, const char* str, D3DCOLOR color = 0xffffffff);
  static void ResetDevice();
  static void LostDevice();
  static void Release();

 private:
  static LPDIRECT3DDEVICE9 d3d9_device_;
  static size_t screen_width_;
  static size_t screen_height_;
  static type_world_to_screen fnWorldToScreen_;
  static type_draw_circle fnDrawCircle_;
  static unsigned char* ebp_ret_;
  static ID3DXLine* line_;
  static LPD3DXFONT font_;
};