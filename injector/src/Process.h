#pragma once
#include <windows.h>
#include <memory>
#include <vector>

class Process {
 public:
  static std::unique_ptr<Process> GetProcessByName(const wchar_t* target_name);
  Process(HANDLE process_handle);
  ~Process();

  DWORD GetModuleBase(const wchar_t* target_name, size_t* out_module_size = nullptr) const;
  bool DumpCodeSectionFromModule(DWORD module_base,
                                 size_t module_size,
                                 std::vector<char>& out_buffer,
                                 DWORD* code_section_addr = nullptr) const;
  DWORD VirtualAlloc(DWORD address, size_t size, DWORD allocation_type, DWORD protect) const;
  bool VirtualProtect(DWORD addr, size_t size, DWORD new_protect) const;
  bool WriteProcMem(DWORD addr, void* buffer, size_t size) const;
  bool MemSet(DWORD addr, unsigned char value, size_t size) const;
  bool CreateThreadAndWait(DWORD start_addr) const;

  template <typename T>
  size_t ReadProcMem(DWORD addr, T* obj, size_t obj_size) const {
    if (!process_handle_) {
      return 0;
    }

    DWORD nb_bytes_read = 0;
    bool success = ReadProcessMemory(process_handle_, reinterpret_cast<PVOID>(addr), obj, obj_size, &nb_bytes_read);
    if (!success) {
      return 0;
    }
    return nb_bytes_read;
  }

 private:
  HANDLE process_handle_;
};