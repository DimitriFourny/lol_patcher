#include "GameApi.h"

float* GameApi::game_time_ = nullptr;

// static
void GameApi::Initialize(float* game_time) {
  game_time_ = game_time;
}
