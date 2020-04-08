#pragma once
#include <windows.h>
#include <vector>

class ExternConfig {
 public:
  ExternConfig();
  std::vector<char> GetConfigBlob() const;

  void fn_end_scene(DWORD v) { config_.fn_end_scene = v; }
  void fn_reset(DWORD v) { config_.fn_reset = v; }
  void kernel32(DWORD v) { config_.kernel32 = v; }
  void d3d9_device(DWORD v) { config_.d3d9_device = v; }
  void objects_manager(DWORD v) { config_.objects_manager = v; }
  void pop_ret(DWORD v) { config_.pop_ret = v; }
  void local_player(DWORD v) { config_.local_player = v; }
  void game_renderer(DWORD v) { config_.game_renderer = v; }
  void world_to_screen(DWORD v) { config_.world_to_screen = v; }
  void draw_circle(DWORD v) { config_.draw_circle = v; }
  void game_time(DWORD v) { config_.game_time = v; }

 private:
#pragma pack(push)
#pragma pack(1)
  struct PackedConfig {
    DWORD fn_end_scene;
    DWORD fn_reset;
    DWORD kernel32;
    DWORD d3d9_device;
    DWORD objects_manager;
    DWORD pop_ret;
    DWORD local_player;
    DWORD game_renderer;
    DWORD world_to_screen;
    DWORD draw_circle;
    DWORD game_time;
  };
#pragma pack(pop)

  PackedConfig config_;
};