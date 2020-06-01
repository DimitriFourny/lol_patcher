#include "Signature.h"
#include "Debug.h"

Signature::Signature(std::vector<char> dump, DWORD dump_rva) {
  dump_ = dump;
  dump_rva_ = dump_rva;
}

const BYTE* Signature::SearchPattern(BYTE* pattern, const char* mask, size_t mask_len) const {
  const BYTE* data = reinterpret_cast<const BYTE*>(dump_.data());
  const BYTE* limit = data + dump_.size();

  size_t pos = 0;
  while (data + pos < limit) {
    if (pos == mask_len) {
      return data;
    }

    if (mask[pos] == '?' || data[pos] == pattern[pos]) {
      pos++;
      continue;
    }
    data++;
    pos = 0;
  }

  return nullptr;
}

DWORD Signature::GetD3DDevice(const Process* process) const {
  // CreateIndexBuffer
  // A1 ? ? ? ? 33 D2 56 57 8B 7C 24 14
  BYTE pattern[] = {0xa1, 0, 0, 0, 0, 0x33, 0xd2, 0x56, 0x57, 0x8b, 0x7c, 0x24, 0x14};
  char mask[] = "x????xxxxxxxx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetD3DDevice: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("CreateIndexBuffer pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  DWORD d3d_renderer_p = 0;
  if (!process->ReadProcMem(sig_addr + 1, &d3d_renderer_p, sizeof(d3d_renderer_p))) {
    debug_printf("Cannot read D3D renderer pointer\n");
    return 0;
  }
  debug_printf("d3d_renderer_p = 0x%08X\n", d3d_renderer_p);

  DWORD d3d_renderer = 0;
  if (!process->ReadProcMem(d3d_renderer_p, &d3d_renderer, sizeof(d3d_renderer))) {
    debug_printf("Cannot read D3D renderer\n");
    return 0;
  }
  debug_printf("d3d_renderer = 0x%08X\n", d3d_renderer);

  DWORD d3d_manager_p = d3d_renderer + 0x18;
  debug_printf("d3d_manager_p = 0x%08X\n", d3d_manager_p);

  DWORD d3d_manager = 0;
  if (!process->ReadProcMem(d3d_manager_p, &d3d_manager, sizeof(d3d_manager))) {
    debug_printf("Cannot read D3D manager\n");
    return 0;
  }
  debug_printf("d3d_manager = 0x%08X\n", d3d_manager);

  DWORD d3d_device_p = d3d_manager + 0x208;
  debug_printf("d3d_device_p = 0x%08X\n", d3d_device_p);

  DWORD d3d_device = 0;
  if (!process->ReadProcMem(d3d_device_p, &d3d_device, sizeof(d3d_device))) {
    debug_printf("Cannot read D3D device\n");
    return 0;
  }

  return d3d_device;
}

DWORD Signature::GetObjectsManager(const Process* process) const {
  // ObjectManager
  // 8B 0D ? ? ? ? E8 ? ? ? ? FF 77
  BYTE pattern[] = {0x8b, 0x0d, 0, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xff, 0x77};
  char mask[] = "xx????x????xx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetObjectsManager: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  DWORD obj_manager_p = 0;
  if (!process->ReadProcMem(sig_addr + 2, &obj_manager_p, sizeof(obj_manager_p))) {
    debug_printf("Cannot read object manager pointer\n");
    return 0;
  }
  debug_printf("obj_manager_p = 0x%08X\n", obj_manager_p);

  DWORD object_manager = 0;
  if (!process->ReadProcMem(obj_manager_p, &object_manager, sizeof(object_manager))) {
    debug_printf("Cannot read object manager renderer\n");
    return 0;
  }
  debug_printf("object_manager = 0x%08X\n", object_manager);

  return object_manager;
}

DWORD Signature::GetLocalPlayer(const Process* process) const {
  // LocalPlayer
  // A1 ? ? ? ? ? ? ? ? 85 C0 74 07 05 ? ? ? ? ? ? ? ? EB 02 33 C0 56
  BYTE pattern[] = {0xA1, 0x00, 0x00, 0x00, 0x00, 0x85, 0xc0, 0x74, 0x07, 0x05};
  char mask[] = "x????xxxxx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetLocalPlayer: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetLocalPlayer: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  DWORD local_player_p = 0;
  if (!process->ReadProcMem(sig_addr + 1, &local_player_p, sizeof(local_player_p))) {
    debug_printf("Cannot read local_player_p\n");
    return 0;
  }
  debug_printf("local_player_p = 0x%08X\n", local_player_p);

  DWORD local_player = 0;
  if (!process->ReadProcMem(local_player_p, &local_player, sizeof(local_player))) {
    debug_printf("Cannot read local_player\n");
    return 0;
  }
  debug_printf("local_player = 0x%08X\n", local_player);

  return local_player;
}

DWORD Signature::GetDrawCircle(const Process* process) const {
  // DrawCircle
  // 81 ec 84 00 00 00 a1 ? ? ? ? 33 c4 89 ? ? ? 00 00 00
  BYTE pattern[] = {0x81, 0xec, 0x84, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00, 0x00,
                    0x33, 0xc4, 0x89, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00, 0xf3, 0x0f};
  char mask[] = "xxxxxxx????xxx???xxxxx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetDrawCircle: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetDrawCircle: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  return sig_addr;
}

DWORD Signature::GetPopRet(const Process* process) const {
  // pop eax ; ret
  // 58 c3
  BYTE pattern[] = {0x58, 0xc3};
  char mask[] = "xx";
  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetPopRet: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetPopRet: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  return sig_addr;
}

DWORD Signature::GetGameRenderer(const Process* process) const {
  // GameRenderer
  // 8B 15 ? ? ? ? 83 EC 08 F3
  BYTE pattern[] = {0x8b, 0x15, 0x00, 0x00, 0x00, 0x00, 0x83, 0xec, 0x08, 0xf3};
  char mask[] = "xx????xxxx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetGameRenderer: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetGameRenderer: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  DWORD value_p = 0;
  if (!process->ReadProcMem(sig_addr + 2, &value_p, sizeof(value_p))) {
    debug_printf("GetGameRenderer: Cannot read value pointer\n");
    return 0;
  }

  DWORD value = 0;
  if (!process->ReadProcMem(value_p, &value, sizeof(value))) {
    debug_printf("GetGameRenderer: Cannot read value\n");
    return 0;
  }

  return value;
}

DWORD Signature::GetWorldToScreen(const Process* process) const {
  // GameRenderer
  // 83 EC 10 56 E8 ? ? ? ? 8B 08
  BYTE pattern[] = {0x83, 0xec, 0x10, 0x56, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x08};
  char mask[] = "xxxxx????xx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetWorldToScreen: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetWorldToScreen: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  return sig_addr;
}

DWORD Signature::GetGameTime(const Process* process) const {
  // GameTime
  // F3 0F 11 05 ? ? ? ? 8B 49
  BYTE pattern[] = {0xf3, 0x0f, 0x11, 0x05, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x49};
  char mask[] = "xxxx????xx";

  const BYTE* sig = SearchPattern(pattern, mask, _countof(mask) - 1);
  if (!sig) {
    debug_printf("GetGameTime: Signature not found\n");
    return 0;
  }

  DWORD sig_offset = reinterpret_cast<const char*>(sig) - dump_.data();
  DWORD sig_addr = dump_rva_ + sig_offset;
  debug_printf("GetGameTime: Pattern found at offset +0x%X (0x%08X)\n", sig_offset, sig_addr);

  DWORD value_p = 0;
  if (!process->ReadProcMem(sig_addr + 4, &value_p, sizeof(value_p))) {
    debug_printf("GetGameTime: Cannot read value pointer\n");
    return 0;
  }

  return value_p;
}