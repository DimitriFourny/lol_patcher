#pragma once
#include <windows.h>
#include <d3dx9.h>

#define DEFINE_SETTERS(type, name, offset) \
  type Get##name() { return *reinterpret_cast<type*>(this + offset); }
#define DEFINE_SETTERS_PTR(type, name, offset) \
  type* Get##name() { return reinterpret_cast<type*>(this + offset); }

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

class SpellInfo {
 public:
  DEFINE_SETTERS_PTR(GameString, Name, 0x18)
};

class SpellSlot {
 public:
  DEFINE_SETTERS(DWORD, Level, 0x20)
  DEFINE_SETTERS(float, CooldownBackTime, 0x28)
  DEFINE_SETTERS(float, Cooldown, 0x78)
  DEFINE_SETTERS(float, Damage, 0x90)
  DEFINE_SETTERS(SpellInfo*, Info, 0x134)
};

class SpellBook {
 public:
  enum class SpellType {
    SpellQ,
    SpellW,
    SpellE,
    SpellR,
    Summon1,
    Summon2,
    Item1,
    Item2,
    Item3,
    Item4,
    Item5,
    Item6,
    Trinket,
    Recall
  };

  SpellSlot* GetSpellSlot(SpellType type) {
    SpellSlot** slots = reinterpret_cast<SpellSlot**>(this + 0x508);
    return slots[static_cast<size_t>(type)];
  }
};

struct Vector3;
class GameObject {
 public:
  DEFINE_SETTERS(char, Index, 0x20)
  DEFINE_SETTERS(BYTE, Team, 0x4C)
  DEFINE_SETTERS_PTR(GameString, Name, 0x6C)
  DEFINE_SETTERS(char, NetworkID, 0xCC)
  DEFINE_SETTERS_PTR(Vector3, Position, 0x1D8)
  DEFINE_SETTERS(char, Visibility, 0x450)
  DEFINE_SETTERS(float, Health, 0xF90)
  DEFINE_SETTERS(float, MaxHealth, 0xFA0)
  DEFINE_SETTERS(char, Lethality, 0x137C)
  DEFINE_SETTERS(float, BonusAtk, 0x13BC)
  DEFINE_SETTERS(float, BaseAtk, 0x143C)
  DEFINE_SETTERS(float, Armor, 0x1460)
  DEFINE_SETTERS(float, BonusArmor, 0x1464)
  DEFINE_SETTERS(char, MagicRes, 0x1468)
  DEFINE_SETTERS(char, BonusMagicRes, 0x146C)
  DEFINE_SETTERS(float, MoveSpeed, 0x1478)
  DEFINE_SETTERS(float, AtkRange, 0x1484)
  DEFINE_SETTERS_PTR(SpellBook, SpellBook, 0x2AB8)
  DEFINE_SETTERS_PTR(char, ChampionName, 0x354C)
  DEFINE_SETTERS(char, Level, 0x4D0C)

  static size_t ObjectSize() { return 0x4D0C + 0x4; }

  DWORD type() {
    DWORD base = reinterpret_cast<DWORD>(this);
    DWORD type_index = *reinterpret_cast<BYTE*>(base + 0x58);
    DWORD general_type =
        *reinterpret_cast<DWORD*>(base + 0x5c + type_index * 4);

    size_t nb = *reinterpret_cast<BYTE*>(base + 0x51);
    if (nb) {
      DWORD xor_value = *reinterpret_cast<DWORD*>(base + 0x54);
      general_type ^= ~xor_value;
    }

    return general_type;
  }

  bool IsMinion() { return (type() & 0x800) && GetName()->c_str()[0] == 'M'; }
  bool IsHero() { return type() & 0x1000; }
  bool IsTurret() { return type() & 0x6000; }
};

struct ObjectManager {
  void* vtable;
  DWORD unk;
  GameObject** objects;
  DWORD max_objects;
  DWORD used_objects;
  DWORD nb_objects;
};

class GameRenderer {
 public:
  DEFINE_SETTERS(DWORD, ScreenWidth, 0x14)
  DEFINE_SETTERS(DWORD, ScreenHeight, 0x18)
  DEFINE_SETTERS_PTR(D3DXMATRIX, ViewMatrix, 0x68)
  DEFINE_SETTERS_PTR(D3DXMATRIX, ProjectionMatrix, 0xA8)
};

#undef DEFINE_SETTERS
#undef DEFINE_SETTERS_PTR