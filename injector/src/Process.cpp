#include "Process.h"
#define NO_DEBUG
#include "Debug.h"

#include <psapi.h>

// static
std::unique_ptr<Process> Process::GetProcessByName(const wchar_t* target_name) {
  DWORD pids[1024];
  DWORD cb_needed = 0;
  if (!EnumProcesses(pids, sizeof(pids), &cb_needed)) {
    debug_printf("EnumProcesses error\n");
    return NULL;
  }

  HANDLE process;
  HMODULE module;
  wchar_t process_name[MAX_PATH] = {0};
  const DWORD nb_process = cb_needed / sizeof(*pids);

  for (DWORD i = 0; i < nb_process; i++) {
    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE,
                          pids[i]);
    if (!process) {
      continue;
    }
    if (!EnumProcessModules(process, &module, sizeof(module), &cb_needed) || !cb_needed) {
      CloseHandle(process);
      continue;
    }

    // Only the first module is interesting at the moment
    process_name[0] = '\0';
    if (!GetModuleBaseNameW(process, module, process_name, _countof(process_name))) {
      continue;
    }

    if (_wcsicmp(process_name, target_name) == 0) {
      return std::make_unique<Process>(process);
    }

    CloseHandle(process);
  }

  return NULL;
}

Process::Process(HANDLE process_handle) : process_handle_(process_handle) {}
Process::~Process() {
  CloseHandle(process_handle_);
}

DWORD Process::GetModuleBase(const wchar_t* target_name, size_t* out_module_size) const {
  if (!process_handle_) {
    return 0;
  }

  HMODULE modules[1024];
  DWORD cb_needed = 0;
  bool success = EnumProcessModules(process_handle_, modules, sizeof(modules), &cb_needed);
  if (!success || !cb_needed) {
    return 0;
  }

  const DWORD nb_modules = cb_needed / sizeof(modules[0]);
  wchar_t module_name[MAX_PATH] = {0};
  for (size_t i = 0; i < nb_modules; i++) {
    if (!GetModuleBaseNameW(process_handle_, modules[i], module_name, _countof(module_name))) {
      return 0;
    }

    if (_wcsicmp(module_name, target_name) == 0) {
      MODULEINFO modinfo;
      if (!GetModuleInformation(process_handle_, modules[i], &modinfo, sizeof(modinfo))) {
        return 0;
      }

      if (out_module_size) {
        *out_module_size = modinfo.SizeOfImage;
      }
      return reinterpret_cast<DWORD>(modinfo.lpBaseOfDll);
    }
  }

  return 0;
}

bool Process::DumpCodeSectionFromModule(DWORD module_base,
                                        size_t module_size,
                                        std::vector<char>& out_buffer,
                                        DWORD* code_section_addr) const {
  if (!process_handle_) {
    return false;
  }
  out_buffer.clear();

  IMAGE_DOS_HEADER dos_header;
  if (!ReadProcMem(module_base, &dos_header, sizeof(dos_header))) {
    debug_printf("DumpCodeSectionFromModule: Can't read DOS header\n");
    return false;
  }
  if (dos_header.e_magic != 0x5a4d) {
    debug_printf("DumpCodeSectionFromModule: Invalid DOS signature\n");
    return false;
  }

  IMAGE_NT_HEADERS32 nt_headers;
  if (!ReadProcMem(module_base + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers))) {
    debug_printf("DumpCodeSectionFromModule: Can't read NT headers\n");
    return false;
  }
  if (nt_headers.Signature != 0x00004550) {
    debug_printf("DumpCodeSectionFromModule: Invalid NT signature\n");
    return false;
  }
  debug_printf("Numbers of section: %d\n", nt_headers.FileHeader.NumberOfSections);
  debug_printf("Base of code: %d\n", nt_headers.OptionalHeader.BaseOfCode);

  auto section_table = std::make_unique<IMAGE_SECTION_HEADER[]>(nt_headers.FileHeader.NumberOfSections);
  bool success = ReadProcMem(module_base + dos_header.e_lfanew + sizeof(nt_headers), section_table.get(),
                             sizeof(IMAGE_SECTION_HEADER) * nt_headers.FileHeader.NumberOfSections);
  if (!success) {
    debug_printf("DumpCodeSectionFromModule: Can't get the sections\n");
    return false;
  }

  IMAGE_SECTION_HEADER* section_code = nullptr;
  for (size_t i = 0; i < nt_headers.FileHeader.NumberOfSections; i++) {
    if (section_table[i].VirtualAddress == nt_headers.OptionalHeader.BaseOfCode) {
      section_code = &section_table[i];
    }
  }
  if (!section_code) {
    debug_printf("DumpCodeSectionFromModule: Code section not found\n");
    return false;
  }

  DWORD code_addr = module_base + section_code->VirtualAddress;
  DWORD code_size = section_code->SizeOfRawData;
  DWORD code_limit_addr = code_addr + code_size;
  debug_printf("Section code of size 0x%x at [0x%08X - 0x%08X]\n", code_size, code_addr, code_limit_addr);

  if (code_section_addr) {
    *code_section_addr = code_addr;
  }

  // Now dump all the memory segments inside the code section which have a
  // different protection that NO_ACCESS
  MEMORY_BASIC_INFORMATION meminfo;
  DWORD target_addr = code_addr;
  DWORD allocation_base = 0;
  DWORD allocation_size = 0;

  while (target_addr < code_limit_addr) {
    if (!VirtualQueryEx(process_handle_, reinterpret_cast<LPCVOID>(target_addr), &meminfo, sizeof(meminfo))) {
      debug_printf("Cannot get memory info at 0x%08X", target_addr);
      return false;
    }
    allocation_base = reinterpret_cast<DWORD>(meminfo.BaseAddress);
    allocation_size = meminfo.RegionSize;
    char* temp_buffer = new char[allocation_size];
    if (meminfo.State & MEM_COMMIT && meminfo.Protect != PAGE_NOACCESS) {
      if (!ReadProcMem(allocation_base, temp_buffer, allocation_size)) {
        debug_printf("Failed to dump 0x%08X\n", allocation_base);
        return false;
      }
    } else {
      memset(temp_buffer, 0, allocation_size);
    }

    out_buffer.insert(out_buffer.end(), temp_buffer, temp_buffer + allocation_size);
    delete[] temp_buffer;
    target_addr += allocation_size;
  }

  return true;
}

DWORD Process::VirtualAlloc(DWORD address, size_t size, DWORD allocation_type, DWORD protect) const {
  void* result = VirtualAllocEx(process_handle_, reinterpret_cast<LPVOID>(address), size, allocation_type, protect);
  return reinterpret_cast<DWORD>(result);
}

bool Process::VirtualProtect(DWORD addr, size_t size, DWORD new_protect) const {
  DWORD old_protect;
  return VirtualProtectEx(process_handle_, reinterpret_cast<LPVOID>(addr), size, new_protect, &old_protect);
}

bool Process::WriteProcMem(DWORD addr, void* buffer, size_t size) const {
  SIZE_T bytes_written = 0;
  bool success = WriteProcessMemory(process_handle_, reinterpret_cast<LPVOID>(addr), buffer, size, &bytes_written);
  return success && bytes_written > 0;
}

bool Process::MemSet(DWORD addr, unsigned char value, size_t size) const {
  unsigned char* block = new unsigned char[size];
  memset(block, value, size);
  bool success = WriteProcMem(addr, block, size);
  delete[] block;
  return success;
}

bool Process::CreateThreadAndWait(DWORD start_addr) const {
  HANDLE thread =
      CreateRemoteThread(process_handle_, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(start_addr), NULL, 0, NULL);
  if (thread) {
    WaitForSingleObject(thread, INFINITE);
  }
  return thread;
}