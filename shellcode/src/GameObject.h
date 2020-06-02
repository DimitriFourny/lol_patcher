#pragma once
#include <d3dx9.h>
#include <windows.h>
#include "GameString.h"
#include "SpellBook.h"

#define DEFINE_SETTERS(type, name, offset) \
  type Get##name() { return *reinterpret_cast<type*>(this + offset); }
#define DEFINE_SETTERS_PTR(type, name, offset) \
  type* Get##name() { return reinterpret_cast<type*>(this + offset); }

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

  DWORD type() {
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

#undef DEFINE_SETTERS
#undef DEFINE_SETTERS_PTR