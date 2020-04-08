#pragma once
#include "GameObject.h"

class GameLoop {
 public:
  static void Update(ObjectManager* object_manager, GameObject* local_player);

  static ObjectManager* object_manager() { return object_manager_; }
  static GameObject* local_player() { return local_player_; }
  static size_t nb_minions() { return nb_minions_; }
  static size_t nb_turrets() { return nb_turrets_; }
  static size_t nb_heroes() { return nb_heroes_; }

  static GameObject* minion(size_t index) {
    if (index >= nb_minions_) {
      return nullptr;
    }
    return minions_[index];
  }
  static GameObject* turret(size_t index) {
    if (index >= nb_turrets_) {
      return nullptr;
    }
    return turrets_[index];
  }
  static GameObject* heroe(size_t index) {
    if (index >= nb_heroes_) {
      return nullptr;
    }
    return heroes_[index];
  }

 private:
  static ObjectManager* object_manager_;
  static GameObject* local_player_;
  static GameObject* minions_[100];
  static GameObject* turrets_[100];
  static GameObject* heroes_[20];
  static size_t nb_minions_;
  static size_t nb_turrets_;
  static size_t nb_heroes_;
};