#include "Draw.h"
#include "Math.h"
#include "WinApi.h"

extern "C" void __cdecl FakingCallRet();

LPDIRECT3DDEVICE9 Draw::d3d9_device_ = nullptr;
size_t Draw::screen_width_ = 0;
size_t Draw::screen_height_ = 0;
type_world_to_screen Draw::fnWorldToScreen_ = nullptr;
type_draw_circle Draw::fnDrawCircle_ = nullptr;
unsigned char* Draw::ebp_ret_ = nullptr;
ID3DXLine* Draw::line_ = nullptr;
LPD3DXFONT Draw::font_ = nullptr;

// static
void Draw::Initialize(LPDIRECT3DDEVICE9 d3d9_device,
                 size_t screen_width,
                 size_t screen_height,
                 type_world_to_screen world_to_screen,
                 type_draw_circle draw_circle) {
  d3d9_device_ = d3d9_device;
  screen_width_ = screen_width;
  screen_height_ = screen_height;
  fnWorldToScreen_ = world_to_screen;
  fnDrawCircle_ = draw_circle;
  line_ = 0;
  font_ = 0;

  // Search: mov ebp esp; pop ebp; ret
  unsigned char* ret_inst = reinterpret_cast<unsigned char*>(fnWorldToScreen_);
  while (*(unsigned long*)ret_inst != 0xc35de58b) {
    ret_inst++;
  }
  ebp_ret_ = ret_inst;
}

// static
bool Draw::WorldToScreen(Vector3* pos, Vector2* pos2D) {
  Vector3 result;

  auto fnWorldToScreenRop =
      reinterpret_cast<void(__cdecl*)(type_world_to_screen, void*, Vector3*,
                                      Vector3*)>(FakingCallRet);

  fnWorldToScreenRop(fnWorldToScreen_, ebp_ret_, pos, &result);

  pos2D->x = result.x;
  pos2D->y = result.y;

  if (result.x < 0.f || result.x > screen_width_) {
    return false;
  }
  if (result.y < 0.f || result.y > screen_height_) {
    return false;
  }

  return true;
}

// static
void Draw::DrawLine(Vector2 start,
                    Vector2 end,
                    float width,
                    bool antialias,
                    D3DCOLOR color) {
  if (!line_) {
    WinApi::D3DXCreateLine(d3d9_device_, &line_);
  }
  line_->SetWidth(width);
  if (antialias) {
    line_->SetAntialias(1);
  }

  D3DXVECTOR2 line[] = {start.ToD3DX(), end.ToD3DX()};
  line_->Begin();
  line_->Draw(line, 2, color);
  line_->End();
}

// static
void Draw::DrawCircle3D(Vector3 pos, float radius, GameColor color) {
  auto fnDrawCircleRop = reinterpret_cast<void(
      __cdecl*)(type_draw_circle, void*, Vector3* position, float range,
                GameColor* color, int a4, float a5, int a6, float alpha)>(
      FakingCallRet);

  fnDrawCircleRop(fnDrawCircle_, ebp_ret_, &pos, radius, &color, 0, 0, 0, 1.f);
}

// static
bool Draw::DrawText2D(Vector2 pos, const char* str, D3DCOLOR color) {
  if (!font_) {
    HRESULT result = WinApi::D3DXCreateFont(
        d3d9_device_, 24, 0, FW_NORMAL, 0, 0, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &font_);
    if (result != S_OK || !font_) {
      return false;
    }
  }

  RECT rect;
  rect.left = static_cast<LONG>(pos.x);
  rect.top = static_cast<LONG>(pos.y);
  rect.right = rect.left + 400;
  rect.bottom = rect.top + 400;
  font_->DrawTextA(NULL, str, -1, &rect, DT_LEFT | DT_NOCLIP, color);
  return true;
}

// static
void Draw::LostDevice() {
  if (font_) {
    font_->OnLostDevice();
  }
  if (line_) {
    line_->OnLostDevice();
  }
}

// static
void Draw::ResetDevice() {
  if (font_) {
    font_->OnResetDevice();
  }
  if (line_) {
    line_->OnResetDevice();
  }
}

// static
void Draw::Release() {
  if (font_) {
    font_->Release();
    font_ = nullptr;
  }
  if (line_) {
    line_->Release();
    line_ = nullptr;
  }
}