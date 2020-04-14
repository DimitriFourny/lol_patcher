#include "main.h"

#include <d3dx9core.h>
#include "GameObject.h"
#include "Math.h"
#include "PEHeaders.h"
#include "string.h"
#include "printf.h"
#include "WinApi.h"
#include "Log.h"
#include "GameLoop.h"
#include "Esp.h"
#include "GameApi.h"

__declspec(dllexport) ExternConfig extern_config;

void DrawPlayerInfo() {
  char str[1024];  // for sprintf

  GameObject* local_player = extern_config.local_player;
  float screen_width =
      static_cast<float>(extern_config.game_renderer->GetScreenWidth());
  Vector2 text_pos(screen_width - 400, 100);

  sprintf(str, "%.f", *extern_config.game_time);
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  Draw::DrawText2D(text_pos, local_player->GetName()->c_str());
  text_pos.y += 30;

  sprintf(str, "Health: %.f/%.f", local_player->GetHealth(),
          local_player->GetMaxHealth());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  sprintf(str, "Attack + Bonus: %.f + %.f", local_player->GetBaseAtk(),
          local_player->GetBonusAtk());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  SpellBook* spellbook = local_player->GetSpellBook();
  SpellSlot* spell = nullptr;

  spell = spellbook->GetSpellSlot(SpellBook::SpellType::SpellQ);
  sprintf(str, "Q = %d (%s) | %.f", spell->GetLevel(),
          spell->GetInfo()->GetName()->c_str(), spell->GetCooldownBackTime());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  spell = spellbook->GetSpellSlot(SpellBook::SpellType::SpellW);
  sprintf(str, "W = %d (%s) | %.f", spell->GetLevel(),
          spell->GetInfo()->GetName()->c_str(), spell->GetCooldownBackTime());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  spell = spellbook->GetSpellSlot(SpellBook::SpellType::SpellE);
  sprintf(str, "E = %d (%s) | %.f", spell->GetLevel(),
          spell->GetInfo()->GetName()->c_str(), spell->GetCooldownBackTime());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 30;

  spell = spellbook->GetSpellSlot(SpellBook::SpellType::SpellR);
  sprintf(str, "R = %d (%s) | %.f", spell->GetLevel(),
          spell->GetInfo()->GetName()->c_str(), spell->GetCooldownBackTime());
  Draw::DrawText2D(text_pos, str);
  text_pos.y += 60;
}

void Init(LPDIRECT3DDEVICE9 device) {
  WinApi::Initialize(extern_config.kernel32);
  Log::Initialize();
  GameApi::Initialize(extern_config.game_time);
  Draw::Initialize(device, extern_config.game_renderer->GetScreenWidth(),
                   extern_config.game_renderer->GetScreenHeight(),
                   extern_config.world_to_screen, extern_config.draw_circle);

  Log::LogStr("Injected :D\n");
  Log::LogStr("Dump the configuration:\n");
  Log::LogPrintf("  &extern_config:  0x%p\n", &extern_config);
  Log::LogPrintf("  fn_end_scene:    0x%p\n", extern_config.fn_end_scene);
  Log::LogPrintf("  fn_reset:        0x%p\n", extern_config.fn_reset);
  Log::LogPrintf("  kernel32:        0x%p\n", extern_config.kernel32);
  Log::LogPrintf("  d3d9_device:     0x%p\n", extern_config.d3d9_device);
  Log::LogPrintf("  objects_manager: 0x%p\n", extern_config.objects_manager);
  Log::LogPrintf("  pop_ret:         0x%p\n", extern_config.pop_ret);
  Log::LogPrintf("  local_player:    0x%p\n", extern_config.local_player);
  Log::LogPrintf("  game_renderer:   0x%p\n", extern_config.game_renderer);
  Log::LogPrintf("  game_time:       0x%p\n", extern_config.game_time);
  Log::LogStr("\n");
}

bool g_is_init = false;
void OnEndScene(LPDIRECT3DDEVICE9 device) {
  // Save the D3D state and restore ours
  // In fullscreen, only do D3DSBT_VERTEXSTATE
  LPDIRECT3DSTATEBLOCK9 state_block = nullptr;
  device->CreateStateBlock(D3DSBT_ALL, &state_block);

  if (!g_is_init) {
    Init(device);
    g_is_init = true;
  }

  Draw::ResetDevice();

  GameLoop::Update(extern_config.objects_manager, extern_config.local_player);
  Esp::DrawAllEsp();
  DrawPlayerInfo();

  Draw::LostDevice();

  state_block->Apply();
  state_block->Release();
}

HRESULT OnReset(LPDIRECT3DDEVICE9 device,
                D3DPRESENT_PARAMETERS* pres_parameters) {
  HRESULT result = extern_config.fn_reset(device, pres_parameters);
  return result;
}

__declspec(dllexport) HRESULT WINAPI HkEndScene(LPDIRECT3DDEVICE9 device) {
  OnEndScene(device);
  return extern_config.fn_end_scene(device);
}

__declspec(dllexport) HRESULT WINAPI
    HkReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* pres_parameters) {
  return OnReset(device, pres_parameters);
}