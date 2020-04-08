#pragma once
#include <windows.h>

class PEHeaders {
 public:
  /**
    Equivalent of the win32 api GetProcAddress().
  */
  static void* GetFuncAddr(void* dll, const char* function_name);
  static void* GetFuncAddr(void* dll, DWORD function_hash);
};