#include "GameLoop.h"
#include <windows.h>
//#include "Draw.h"

ObjectManager* GameLoop::object_manager_ = nullptr;
GameObject* GameLoop::local_player_ = nullptr;
GameObject* GameLoop::minions_[100] = {0};
GameObject* GameLoop::turrets_[100] = {0};
GameObject* GameLoop::heroes_[20] = {0};
size_t GameLoop::nb_minions_ = 0;
size_t GameLoop::nb_turrets_ = 0;
size_t GameLoop::nb_heroes_ = 0;

// static
void GameLoop::Update(ObjectManager* object_manager, GameObject* local_player) {
  object_manager_ = object_manager;
  local_player_ = local_player;
  nb_minions_ = 0;
  nb_turrets_ = 0;
  nb_heroes_ = 0;
  if (!object_manager_ || !local_player_) {
    return;
  }

  GameObject** game_objects = object_manager_->objects;
  if (!game_objects) {
    return;
  }

  size_t nb_objects = object_manager_->max_objects;
  for (size_t i = 0; i < nb_objects; i++) {
    auto obj = game_objects[i];
    if (!obj) {
      continue;
    }
    /*
    Vector3* pos = obj->GetPosition();
    Vector2 pos2D(0, 0);
    char* name = obj->GetName()->c_str();
    if (pos->x && pos->y && pos->z && name) {
      if (Draw::WorldToScreen(pos, &pos2D)) {
        Draw::DrawText2D(pos2D, name);
      }
    }
    */
    if (obj->IsMinion()) {  // Minion
      if (!obj->GetHealth()) {
        continue;
      }
      if (nb_minions_ >= _countof(minions_)) {
        continue;
      }
      minions_[nb_minions_++] = obj;
    } else if (obj->IsTurret()) {  // Turret
      if (!obj->GetHealth()) {
        continue;
      }
      if (nb_turrets_ >= _countof(turrets_)) {
        continue;
      }
      turrets_[nb_turrets_++] = obj;
    } else if (obj->IsHero()) {  // Hero
      if (!obj->GetHealth()) {
        continue;
      }
      if (nb_heroes_ >= _countof(heroes_)) {
        continue;
      }
      heroes_[nb_heroes_++] = obj;
    }
  }
}