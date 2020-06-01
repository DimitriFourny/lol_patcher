#include <windows.h>
#include "D3dHook.h"
#include "Debug.h"
#include "Shellcode.h"

void DumpBuffer(unsigned char* buffer, size_t size) {
  printf("00: ");
  for (size_t i = 0; i < size; i++) {
    printf("%02x ", buffer[i]);
    if (i + 1 != size && (i + 1) % 16 == 0) {
      printf("\n%02X: ", i + 1);
    }
  }
  printf("\n");
}

int main() {
  const wchar_t process_name[] = L"League of legends.exe";
  std::shared_ptr<Process> process = Process::GetProcessByName(process_name);
  if (!process) {
    debug_printf("[-] No LoL process\n");
    return 1;
  }

  size_t module_size = 0;
  DWORD module_base = process->GetModuleBase(process_name, &module_size);
  debug_printf("Module base = 0x%08X\n", module_base);
  if (!module_base) {
    return 1;
  }

  std::vector<char> module_dump;
  DWORD code_section_addr;
  bool success = process->DumpCodeSectionFromModule(module_base, module_size, module_dump, &code_section_addr);
  if (!success) {
    debug_printf("Failed to dump the module\n");
    return 1;
  }
  debug_printf("Dumped a code section of 0x%x bytes from 0x%08X\n", module_dump.size(), code_section_addr);

  Shellcode shellcode;
  if (!shellcode.Map()) {
    debug_printf("Cannot map the shellcode\n");
    return 1;
  }

  Signature signature(module_dump, code_section_addr);
  D3dHook d3d_hook(process);
  success = d3d_hook.LoadDeviceVtable(&signature);
  if (!success) {
    return 1;
  }

  ExternConfig config;
  config.fn_end_scene(d3d_hook.GetMemberAddr(D3dHook::D3dVtableMembers::EndScene));
  config.fn_reset(d3d_hook.GetMemberAddr(D3dHook::D3dVtableMembers::Reset));
  config.kernel32(process->GetModuleBase(L"kernel32.dll"));
  config.d3d9_device(d3d_hook.d3d9_device());
  config.objects_manager(signature.GetObjectsManager(process.get()));
  config.pop_ret(signature.GetPopRet(process.get()));
  config.local_player(signature.GetLocalPlayer(process.get()));
  config.game_renderer(signature.GetGameRenderer(process.get()));

  DWORD world_to_screen = signature.GetWorldToScreen(process.get());
  if (!world_to_screen) {
    return 1;
  }
  config.world_to_screen(world_to_screen);

  DWORD draw_circle = signature.GetDrawCircle(process.get());
  if (!draw_circle) {
    return 1;
  }
  config.draw_circle(draw_circle);

  DWORD game_time = signature.GetGameTime(process.get());
  if (!game_time) {
    return 1;
  }
  config.game_time(game_time);

  DWORD new_end_scene = 0;
  DWORD new_reset = 0;
  success = shellcode.InjectInProcess(process.get(), &config, &new_end_scene, &new_reset);
  if (!success) {
    printf("Cannot inject the shellcode\n");
  }

  d3d_hook.MoveVtable();
  d3d_hook.HookMember(D3dHook::D3dVtableMembers::EndScene, new_end_scene);
  d3d_hook.HookMember(D3dHook::D3dVtableMembers::Reset, new_reset);

  printf("Press a key to unload the hooks...\n");
  getchar();
  d3d_hook.RestoreVtable();

  return 0;
}
