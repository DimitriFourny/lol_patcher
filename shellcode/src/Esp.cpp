#include "Esp.h"
#include "GameLoop.h"
#include "Draw.h"
#include "GameApi.h"

// static
void Esp::DrawEspMinion() {
  GameColor color_last_hit(255, 255, 0);
  for (size_t i = 0; i < GameLoop::nb_minions(); i++) {
    if (GameLoop::minion(i)->GetTeam() == GameLoop::local_player()->GetTeam()) {
      continue;
    }

    // Show only a minion ESP if its life inferior to my base attack
    float attack_damage = GameLoop::local_player()->GetBaseAtk() +
                          GameLoop::local_player()->GetBonusAtk();
    if (GameLoop::minion(i)->GetHealth() <= attack_damage) {
      Draw::DrawCircle3D(*GameLoop::minion(i)->GetPosition(), 100.f,
                         color_last_hit);
    }
  }
}

// static
void Esp::DrawEspTurrets() {
  GameColor color;
  for (size_t i = 0; i < GameLoop::nb_turrets(); i++) {
    if (GameLoop::turret(i)->GetTeam() == GameLoop::local_player()->GetTeam()) {
      color = GameColor(0, 0, 255);
    } else {
      color = GameColor(255, 0, 0);
    }

    Draw::DrawCircle3D(*GameLoop::turret(i)->GetPosition(), 875.f, color);
  }
}

// static
void Esp::DrawEspHeroes() {
  GameColor color(0, 255, 255);
  for (size_t i = 0; i < GameLoop::nb_heroes(); i++) {
    if (GameLoop::heroe(i)->GetTeam() == GameLoop::local_player()->GetTeam()) {
      continue;
    }

    Draw::DrawCircle3D(*GameLoop::heroe(i)->GetPosition(),
                       GameLoop::heroe(i)->GetAtkRange(), color);
  }
}

// static
void Esp::DrawRangeEsp() {
  GameObject* local_player = GameLoop::local_player();

  GameColor color(128, 0, 255);
  Draw::DrawCircle3D(*local_player->GetPosition(), local_player->GetAtkRange(),
                     color);
}

// static
void Esp::DrawEspSpells() {
  GameColor color(0, 255, 255);
  Vector2 pos2d;
  Vector2 text_pos;
  Vector2 cooldown_pos;

  SpellBook::SpellType spell_types[] = {
      SpellBook::SpellType::SpellQ,  SpellBook::SpellType::SpellW,
      SpellBook::SpellType::SpellE,  SpellBook::SpellType::SpellR,
      SpellBook::SpellType::Summon1, SpellBook::SpellType::Summon2,
  };
  SpellBook* spellbook = nullptr;
  SpellSlot* spell_slot = nullptr;
  char spell_str[10] = {0};
  char cooldown_str[10] = {0};
  D3DCOLOR spell_color;
  D3DCOLOR cooldown_color = D3DCOLOR_ARGB(255, 255, 255, 255);

  for (size_t i = 0; i < GameLoop::nb_heroes(); i++) {
    if (!Draw::WorldToScreen(GameLoop::heroe(i)->GetPosition(), &pos2d)) {
      continue;
    }

    // TODO: precise rect and DT_CENTER
    text_pos.x = pos2d.x + 100;
    text_pos.y = pos2d.y - 260;
    spellbook = GameLoop::heroe(i)->GetSpellBook();

    for (size_t spell = 0; spell < _countof(spell_types); spell++) {
      spell_slot = spellbook->GetSpellSlot(spell_types[spell]);
      const int time_still = static_cast<int>(
          spell_slot->GetCooldownBackTime() - GameApi::game_time());

      // Cooldown
      if (spell_slot->GetLevel() && time_still > 0) {
        cooldown_pos = text_pos;

        if (spell % 2 == 0) {
          cooldown_pos.y -= 20;
        } else {
          cooldown_pos.y += 20;
        }

        itoa(time_still, cooldown_str, 10);
        Draw::DrawText2D(cooldown_pos, cooldown_str, cooldown_color);
      }

      // Level
      if (!spell_slot->GetLevel() || time_still > 0) {
        spell_color = D3DCOLOR_ARGB(255, 255, 0, 0);
      } else {
        spell_color = D3DCOLOR_ARGB(255, 255, 255, 255);
      }

      itoa(spell_slot->GetLevel(), spell_str, 10);
      Draw::DrawText2D(text_pos, spell_str, spell_color);
      text_pos.x += 20;
    }
  }
}

// static
void Esp::DrawAllEsp() {
  DrawEspMinion();
  DrawEspTurrets();
  DrawEspHeroes();
  DrawRangeEsp();
  DrawEspSpells();
}