#pragma once

class GameApi {
 public:
  static void Initialize(float* game_time);
  static float game_time() {
    if (!game_time_) {
      return 0;
    }
    return *game_time_;
  }

 private:
  static float* game_time_;
};