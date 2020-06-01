#include "WinApi.h"
#include "Hash.h"
#include "PEHeaders.h"
#include "StringEncryption.h"

type_get_module_handle WinApi::GetModuleHandleA = nullptr;
type_create_file WinApi::CreateFileW = nullptr;
type_write_file WinApi::WriteFile = nullptr;
type_flush_file_buffers WinApi::FlushFileBuffers = nullptr;
type_sleep WinApi::Sleep = nullptr;
type_create_line WinApi::D3DXCreateLine = nullptr;
type_create_font WinApi::D3DXCreateFont = nullptr;

// static
void WinApi::Initialize(void* kernel32) {
  WinApi::GetModuleHandleA =
      reinterpret_cast<type_get_module_handle>(PEHeaders::GetFuncAddr(kernel32, DJB_HASH("GetModuleHandleA")));
  WinApi::CreateFileW = reinterpret_cast<type_create_file>(PEHeaders::GetFuncAddr(kernel32, DJB_HASH("CreateFileW")));
  WinApi::WriteFile = reinterpret_cast<type_write_file>(PEHeaders::GetFuncAddr(kernel32, DJB_HASH("WriteFile")));
  WinApi::FlushFileBuffers =
      reinterpret_cast<type_flush_file_buffers>(PEHeaders::GetFuncAddr(kernel32, DJB_HASH("FlushFileBuffers")));
  WinApi::Sleep = reinterpret_cast<type_sleep>(PEHeaders::GetFuncAddr(kernel32, DJB_HASH("Sleep")));

  XorS(d3d9_dll, "d3dx9_39.dll");
  void* d3dx9 = WinApi::GetModuleHandleA(d3d9_dll.decrypt());

  WinApi::D3DXCreateLine =
      reinterpret_cast<type_create_line>(PEHeaders::GetFuncAddr(d3dx9, DJB_HASH("D3DXCreateLine")));
  WinApi::D3DXCreateFont =
      reinterpret_cast<type_create_font>(PEHeaders::GetFuncAddr(d3dx9, DJB_HASH("D3DXCreateFontW")));
}