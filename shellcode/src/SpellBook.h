#pragma once
#include "GameString.h"

#define DEFINE_SETTERS(type, name, offset) \
  type Get##name() { return *reinterpret_cast<type*>(this + offset); }
#define DEFINE_SETTERS_PTR(type, name, offset) \
  type* Get##name() { return reinterpret_cast<type*>(this + offset); }

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

#undef DEFINE_SETTERS
#undef DEFINE_SETTERS_PTR