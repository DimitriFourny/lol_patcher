#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include "Draw.h"
#include "HudManager.h"

typedef HRESULT(WINAPI* type_end_scene)(LPDIRECT3DDEVICE9 device);
typedef HRESULT(WINAPI* type_reset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

// typedef int*(__thiscall* fnIssueOrder)(void *thisPtr, int Order, Vector *Loc, CObject *Target, bool IsAttackMove, bool IsMinion, DWORD Unknown);
// typedef int* (__thiscall* fnCastSpell)(DWORD spellbook_addr, SpellSlot* spellslot, int SlotID, Vector* targetpos, Vector* startpos, DWORD NetworkID);


class ObjectManager;
class GameObject;
class GameRenderer;

#pragma pack(push)
#pragma pack(1)
struct ExternConfig {
  type_end_scene fn_end_scene;
  type_reset fn_reset;
  void* kernel32;
  LPDIRECT3DDEVICE9 d3d9_device;
  ObjectManager* objects_manager;
  BYTE* pop_ret;
  GameObject* local_player;
  GameRenderer* game_renderer;
  type_world_to_screen world_to_screen;
  type_draw_circle draw_circle;
  float* game_time;
  void* issue_order;
  void* cast_spell;
  HudManager* hud_instance;
};
#pragma pack(pop)
