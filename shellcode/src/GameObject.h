#pragma once
#include <d3dx9.h>
#include <windows.h>

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
    SpellSlot** slots = reinterpret_cast<SpellSlot**>(this + 0x510);
    return slots[static_cast<size_t>(type)];
  }
};

struct Vector3;
class GameObject {
 public:
  DEFINE_SETTERS(BYTE, Team, 0x4C)
  DEFINE_SETTERS_PTR(GameString, Name, 0x6C)
  DEFINE_SETTERS(char, NetworkID, 0xCC)
  DEFINE_SETTERS_PTR(Vector3, Position, 0x1D8)
  DEFINE_SETTERS(float, Health, 0xF88)
  DEFINE_SETTERS(float, MaxHealth, 0xF98)
  DEFINE_SETTERS(float, BonusAtk, 0x13CC)
  DEFINE_SETTERS(float, BaseAtk, 0x143C)
  DEFINE_SETTERS(float, Armor, 0x1460)
  DEFINE_SETTERS(float, BonusArmor, 0x1464)
  DEFINE_SETTERS(char, MagicRes, 0x1468)
  DEFINE_SETTERS(char, BonusMagicRes, 0x146C)
  DEFINE_SETTERS(float, MoveSpeed, 0x147C)
  DEFINE_SETTERS(float, AtkRange, 0x1484)
  DEFINE_SETTERS_PTR(SpellBook, SpellBook, 0x2AD0)
  DEFINE_SETTERS_PTR(char, ChampionName, 0x358C)
  DEFINE_SETTERS(char, Level, 0x4EA4)

  static size_t ObjectSize() { return 0x4EA4 + 0x4; }

  DWORD type() {
    /*edx = 0
      eax = byte[edi+58]
      esi = byte[edi+51]
      var4 = [edi + eax*4 + 0x5c]

      if esi > 0:
        while edx < esi:
          ecx = edi + 54
          eax = [ecx]
          var4[edx*4] ^= ~eax
          ecx += 4
          edx++

      al = byte[edi+52]
      if al > 0:
        ecx = al
        eax = 4 - ecx
        while eax < 4:
          edx = edi + 54 + ecx
          cl = byte[edx]
          edx++
          var4[eax] ^= ~cl
          eax++

      return var4 & arg0 */
    DWORD base = reinterpret_cast<DWORD>(this);
    DWORD type_index = *reinterpret_cast<BYTE*>(base + 0x58);
    DWORD general_type = *reinterpret_cast<DWORD*>(base + 0x5c + type_index * 4);

    // DWORD xor
    size_t nb = *reinterpret_cast<BYTE*>(base + 0x51);
    if (nb) {
      DWORD xor_value = *reinterpret_cast<DWORD*>(base + 0x54);
      general_type ^= ~xor_value;
    }

    // BYTE xor
    nb = *reinterpret_cast<BYTE*>(base + 0x52);
    if (nb && nb <= 4) {
      for (size_t i = 0; i < nb; i++) {
        reinterpret_cast<BYTE*>(general_type)[i] ^= ~reinterpret_cast<BYTE*>(base + 0x54)[i];
      }
    }

    return general_type;
  }

  bool IsMinion() { return (type() & 0x800) && GetName()->c_str()[0] == 'M'; }
  bool IsHero() { return type() & 0x1000; }
  bool IsTurret() { return type() & 0x6000; }
  bool IsValid() { return !(reinterpret_cast<DWORD>(this) & 0x1); }
};

struct ObjectManager {
 public:
  DEFINE_SETTERS(GameObject**, FirstArrayElement, 0x14)
  DEFINE_SETTERS(GameObject**, LastArrayElement, 0x18)

  size_t size() {
    return (reinterpret_cast<DWORD>(GetLastArrayElement()) - reinterpret_cast<DWORD>(GetFirstArrayElement())) /
           sizeof(DWORD);
  }
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