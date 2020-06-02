#pragma once
#include <windows.h>
#include "SpellBook.h"

#define DEFINE_SETTERS(type, name, offset) \
  type Get##name() { return *reinterpret_cast<type*>(this + offset); }
#define DEFINE_SETTERS_PTR(type, name, offset) \
  type* Get##name() { return reinterpret_cast<type*>(this + offset); }

class MouseManager {
 public:
  DEFINE_SETTERS_PTR(Vector3, Position3D, 0x1c)
  DEFINE_SETTERS(SpellInfo*, Spell, 0x84)
};

class HudManager {
 public:
  DEFINE_SETTERS(MouseManager*, Mouse, 0x14)
};

#undef DEFINE_SETTERS
#undef DEFINE_SETTERS_PTR