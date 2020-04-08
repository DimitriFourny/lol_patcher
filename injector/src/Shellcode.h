#pragma once
#include "Process.h"
#include "ExternConfig.h"

class Shellcode {
 public:
  Shellcode();
  bool Map();
  bool InjectInProcess(Process* process,
                       const ExternConfig* config,
                       DWORD* new_end_scene,
                       DWORD* new_reset);
  DWORD GetFunctionRVA(const char* function_name);

 private:
  DWORD Rva2FileOffset(DWORD rva);

  unsigned char* buffer_;
  size_t buffer_size_;
};